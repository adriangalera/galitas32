#include <Arduino.h>
#include <debug.h>
#include "bultinrgbled.h"
#include "door.h"

unsigned long lastEvent = 0;
int phase = 0;

void setup()
{
  setup_debug();
  setup_bultin_rgb_led();
  rgb_led(SUCCESS, 2);
  setup_door_sensor();
}

void loop()
{
  DoorState doorState = read_door_sensor();
  Debugf("Door state: %s\n", doorState == DOOR_OPEN ? "OPEN" : "CLOSED");
  if (doorState == DOOR_OPEN)
  {
    rgb_led(WARNING);
  }
  else
  {
    rgb_led(SUCCESS);
  }
  delay(10);
}