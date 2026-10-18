#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ---------------- Door sensor ----------------
#define DOOR_SENSOR_PIN 12

// ---------------- LED strip ----------------
#define LED_PIN         10
#define LED_COUNT       60

// ---------------- Buzzer ----------------
#define BUZZER_PIN      4



Adafruit_NeoPixel strip(
    LED_COUNT,
    LED_PIN,
    NEO_GRB + NEO_KHZ800
);


// --------------------------------------------------
// Set all LEDs to the same color
// --------------------------------------------------
void setLedColor(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        strip.setPixelColor(
            i,
            strip.Color(red, green, blue)
        );
    }

    strip.show();
}


// --------------------------------------------------
// Setup
// --------------------------------------------------
void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("===== APPLICATION STARTED =====");

    // Door sensor
    //
    // INPUT_PULLUP:
    // LOW  -> switch closed -> door closed
    // HIGH -> switch open   -> door open
    pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);

    // NeoPixel
    strip.begin();
    strip.clear();
    strip.show();

    Serial.println("Setup complete ...");
}


// --------------------------------------------------
// Main loop
// --------------------------------------------------
void loop()
{
    int doorState = digitalRead(DOOR_SENSOR_PIN);

    // Door CLOSED
    if (doorState == LOW)
    {
        Serial.println("Door is closed");

        // BLUE
        setLedColor(0, 0, 255);
        analogWrite(BUZZER_PIN, 200); // turn on
    }

    // Door OPEN
    else
    {
        Serial.println("Door is open");

        // RED
        setLedColor(255, 0, 0);
        analogWrite(BUZZER_PIN, 0); // turn off
    }

    delay(100);
}