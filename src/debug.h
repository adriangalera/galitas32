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
inline void debugBegin(unsigned long baud = 115200) {
#if DEBUG_ENABLED
    Serial.begin(baud);
    while (!Serial) {
        delay(10); // wait for USB serial (safe on ESP32-S3)
    }
#endif
}


// ========================
// Debug macros
// ========================
#if DEBUG_ENABLED

#define DEBUG(x)      Serial.println(x)

#else

#define DEBUG(x)

#endif