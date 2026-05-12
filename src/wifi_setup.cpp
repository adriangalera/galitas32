#include "wifi_setup.h"
#include "debug.h"
#include "secrets.h"


void connect_to_wifi()
{
    Debug("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Debug(".");
    }
    Debug("Connected!");
}