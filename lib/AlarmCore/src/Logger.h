#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#define LOG_DEBUG(...)              \
    do                              \
    {                               \
        Serial.printf(__VA_ARGS__); \
        Serial.println();           \
    } while (0)
#else
#include <stdio.h>
#define LOG_DEBUG(...)       \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
        printf("\n");        \
        fflush(stdout);      \
    } while (0)
#endif