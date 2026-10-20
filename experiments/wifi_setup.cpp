#include "wifi_setup.h"
#include "debug.h"
#include "secrets.h"

// --- FreeRTOS Event Group ---
// Event groups allow us to set bits (flags) that other tasks can wait for.
EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;
const int WIFI_FAIL_BIT      = BIT1;

// Keep track of connection attempts
static int s_retry_num = 0;
#define MAXIMUM_RETRY  5

// --- Wi-Fi Event Handler ---
// The native ESP32 way to handle Wi-Fi is asynchronous via system events
void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            Debug("WiFi Station Started. Connecting...\n");
            WiFi.begin(WIFI_SSID, WIFI_PASS);
            break;
            
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Debug("Connected! IP Address: ");
            Debug(WiFi.localIP().toString().c_str());
            Debug("\n");
            s_retry_num = 0;
            // Signal to all FreeRTOS tasks that the internet is ready
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
            
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Debug("Disconnected from WiFi.\n");
            xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
            
            if (s_retry_num < MAXIMUM_RETRY) {
                vTaskDelay(pdMS_TO_TICKS(2000)); // Wait 2 seconds before retrying
                s_retry_num++;
                Debug("Retrying connection to AP...\n");
                WiFi.begin(WIFI_SSID, WIFI_PASS);
            } else {
                Debug("Failed to connect to WiFi after max retries.\n");
                xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
            }
            break;
        default:
            break;
    }
}

// --- FreeRTOS Wi-Fi Provisioning Task ---
void wifi_setup_task(void *pvParameters) {
    Debug("Initializing Wi-Fi Event Group...\n");
    wifi_event_group = xEventGroupCreate();

    // Register the asynchronous event handler
    WiFi.onEvent(WiFiEvent);

    // Start Wi-Fi station mode (this triggers ARDUINO_EVENT_WIFI_STA_START)
    WiFi.mode(WIFI_MODE_STA);

    /* Block this task until either we connect (WIFI_CONNECTED_BIT) 
       or fail completely (WIFI_FAIL_BIT). 
       pdFALSE: Don't clear bits automatically on exit.
       pdFALSE: Wait for *either* bit, not both.
       portMAX_DELAY: Wait indefinitely without wasting CPU cycles.
    */
    EventBits_t bits = xEventGroupWaitBits(
        wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY 
    );

    if (bits & WIFI_CONNECTED_BIT) {
        Debug("WiFi Thread: Initialization successful, pipeline open.\n");
    } else if (bits & WIFI_FAIL_BIT) {
        Debug("WiFi Thread: Critical Failure. Application moving to offline-mode.\n");
    }

    // Self-terminate the initialization task to free memory, 
    // since system events handle reconnects in the background now.
    vTaskDelete(NULL);
}

void connect_to_wifi() {

    // Launch the Wi-Fi manager task on Core 0 (where the network stack resides)
    xTaskCreatePinnedToCore(
        wifi_setup_task,     // Function to implement the task
        "WiFi_Setup",        // Name of the task
        4096,                // Stack size in bytes 
        NULL,                // Task input parameter
        2,                   // Priority (relatively high during initialization)
        NULL,                // Task handle
        0                    // Core ID (0 is optimized for radio protocols)
    );
}