#include "bultinrgbled.h"
#include "debug.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

static const int PIN_RGB_LED = 48;
static const int BRIGHTNESS = 10;

static Adafruit_NeoPixel pixel(1, PIN_RGB_LED, NEO_GRB + NEO_KHZ800);

struct RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

static const RGB colors[STATE_COUNT] = {
    {0, 0, 0},     // IDLE
    {255, 0, 0},   // ERROR
    {0, 255, 0},   // SUCCESS
    {255, 128, 0}, // WARNING
};

static LedState currentState = IDLE;

/*
    0 means:
    no expiration
*/
static unsigned long stateExpiresAt = 0;

static void apply_state(LedState s)
{
    currentState = s;

    RGB c = colors[s];

    pixel.setBrightness(BRIGHTNESS);
    pixel.setPixelColor(0, pixel.Color(c.r, c.g, c.b));
    pixel.show();
}

void rgb_led(LedState state, int apply_for_sec)
{
    apply_state(state);

    if (apply_for_sec < 0)
    {
        stateExpiresAt = 0;
    }
    else
    {
        stateExpiresAt = millis() + (apply_for_sec * 1000);
    }
}

void rgb_led_loop()
{
    if (stateExpiresAt != 0 &&
        millis() >= stateExpiresAt &&
        currentState != IDLE)
    {
        apply_state(IDLE);
        stateExpiresAt = 0;
    }
}

void setup_bultin_rgb_led()
{
    pixel.begin();
    pixel.clear();
    pixel.show();

    rgb_led(IDLE);
}