#pragma once
#include <Arduino.h>

// ========================
// Enable / Disable Debug
// ========================
#ifndef DEBUG_ENABLED
#define DEBUG_ENABLED 1
#endif

// ========================
// Init
// ========================
inline void setup_debug(unsigned long baud = 115200) {
#if DEBUG_ENABLED
    Serial.begin(baud);
    while (!Serial) {
        delay(10); // wait for USB serial (safe on ESP32-S3)
    }
    Serial.println("Debugger initialized!");
#endif
}


// ========================
// Debug macros
// ========================
#if DEBUG_ENABLED

#define Debug(x)      Serial.println(x)
#define Debugf(...) Serial.printf(__VA_ARGS__)

#else

#define Debug(x)
#define Debugf(...)


#endif