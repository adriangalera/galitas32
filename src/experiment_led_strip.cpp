#include <Adafruit_NeoPixel.h>

#define LED_PIN    20
#define LED_COUNT  60

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int position = 0;
int direction = 1;

void setup() {
    strip.begin();
    strip.clear();
    strip.show();
}

void loop() {

    // clear strip
    strip.clear();


    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(255, i*2, 0));
    }
     strip.show();

    delay(1000);
}