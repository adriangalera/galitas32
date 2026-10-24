#include <Arduino.h>
#include <ArduinoDoorSensor.h>
#include <Alarm.h>
#include <door/DoorDebouncer.h>
#include <config.h>
#include <NeoPixelLed.h>


NeoPixelLed led(LED_PIN, LED_COUNT);
LedController ledController(led);
Alarm galitasAlarm(ledController);
ArduinoDoorSensor doorSensor(DOOR_SENSOR_PIN);
DoorDebouncer doorDebouncer(galitasAlarm, doorSensor);

void setup()
{
    const unsigned long now = millis();
    LOG_DEBUG("Setting up the board and the sensors...");
    Serial.begin(115200);
    doorSensor.setup();
    led.setup();
    doorDebouncer.begin(now);
    LOG_DEBUG("Set up complete at %lu ms", now);
}

void loop()
{
    const unsigned long now = millis();
    doorDebouncer.update(now);
    ledController.update(now);
    delay(1);
}