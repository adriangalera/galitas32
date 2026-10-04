#include <Adafruit_NeoPixel.h>

#define LED_PIN    20
#define LED_COUNT  60

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {

    Serial.begin(115200);
    Serial.println("===== APPLICATION STARTED =====");

    strip.begin();
    strip.clear();
    strip.show();

    Serial.println("Setup complete ...");
}

void loop() {

    // Turn all LEDs ON
    for (int i = 0; i < LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(255, i * 2, 0));
    }

    strip.show();

    delay(1000);

    // Turn all LEDs OFF
    strip.clear();
    strip.show();

    delay(1000);

    Serial.println("Blinking complete ...");
}