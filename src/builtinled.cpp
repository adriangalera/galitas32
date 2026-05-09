#include "builtinled.h"
#include "debug.h"

static int _pin;

static enum {
    IDLE,
    ERROR,
}

state = IDLE;

static unsigned long lastToggle = 0;
static bool level = false;

static uint8_t blinkCount = 0;
static uint8_t targetBlinks = 0;

void setup_bultin_led()
{
    _pin = LED_BUILTIN;
    pinMode(_pin, OUTPUT);
}

void led_error(uint8_t blinks)
{
    state = ERROR;
    blinkCount = 0;
    targetBlinks = blinks;
    lastToggle = millis();
    level = false;
}

void led_loop()
{
    unsigned long now = millis();
    Debugf("LED loop, state: %d\n", state);
    switch (state)
    {

    case ERROR:
        Debugf("In ERROR state, blinkCount: %d, targetBlinks: %d\n", blinkCount, targetBlinks);
        Debugf("Time since last toggle: %lu ms\n", now - lastToggle);
        if (now - lastToggle >= 80)
        {
            lastToggle = now;

            level = !level;
            digitalWrite(_pin, level ? HIGH : LOW);
            Debugf("LED toggled to %s\n", level ? "HIGH" : "LOW");
            if (level)
            {
                blinkCount++;

                if (blinkCount >= targetBlinks)
                {
                    state = IDLE;
                }
            }
        }
        break;

    case IDLE:
    default:
        digitalWrite(_pin, LOW);
        break;
    }
}