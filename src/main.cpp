#include <Arduino.h>
#include <debug.h>
#include "bultinrgbled.h"

unsigned long lastEvent = 0;
int phase = 0;

void setup()
{
  setup_debug();
  setup_bultin_rgb_led();
  rgb_led(SUCCESS, 2);
}

void loop()
{
  rgb_led_loop();

  unsigned long now = millis();

  switch (phase)
  {
  case 0:
    // after 2 seconds -> fail
    if (now > 2000)
    {
      rgb_led(ERROR, 3); // red for 3 sec
      phase = 1;
    }
    break;

  case 1:
    // wait until error expires
    if (now > 7000)
    {
      rgb_led(SUCCESS, 2); // green for 2 sec
      phase = 2;
    }
    break;

  case 2:
    // done
    break;
  }
}