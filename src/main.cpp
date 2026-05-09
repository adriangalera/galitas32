#include <Arduino.h>
#include <debug.h>
#include <builtinled.h>

void setup()
{
  setup_debug();
  setup_bultin_led();
}

void loop()
{
  led_loop();
}