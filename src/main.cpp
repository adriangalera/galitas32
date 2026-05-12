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
  DoorState doorState = read_door_sensor();
  Debugf("Door state: %s\n", doorState == DOOR_OPEN ? "OPEN" : "CLOSED");
  if (doorState == DOOR_OPEN)
  {
    bultin_rgb_led(WARNING);
  }
  else
  {
    bultin_rgb_led(SUCCESS);
  }
  delay(10);
}