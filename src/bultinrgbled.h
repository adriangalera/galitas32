#pragma once

enum LedState
{
    IDLE,
    ERROR,
    SUCCESS,
    WARNING,

    STATE_COUNT
};


void setup_bultin_rgb_led();
void rgb_led(LedState state, int apply_for_sec = 0);
void rgb_led_loop();