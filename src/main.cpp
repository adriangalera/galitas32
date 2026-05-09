#include <Arduino.h>
#include <debug.h>
#include <builtinled.h>

void setup()
{
  setup_debug();
  setup_bultin_led();
}

unsigned long lastErrorTrigger = 0;

void loop()
{
  led_loop();

  if (millis() - lastErrorTrigger > 5000)
  {
    lastErrorTrigger = millis();
    led_error();
  }
}