#include <Arduino.h>
#include <debug.h>
#include "bultinrgbled.h"
#include "door.h"
#include "wifi_setup.h"

void setup()
{
  setup_debug();
  setup_bultin_rgb_led();
  bultin_rgb_led(SUCCESS, 2);
  setup_door_sensor();
  connect_to_wifi();
}

void loop()
{
 
}