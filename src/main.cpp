#include <Arduino.h>

#define DEBUG_ENABLED 1

#include "debug.h"

void setup() {
  debugBegin();
  DEBUG("Debugger initialized");
}

void loop() {
  DEBUG("Hello, ESP32-S3!");
  delay(1000);
}