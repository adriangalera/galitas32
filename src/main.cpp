#include <Arduino.h>
#include "ArduinoDoorSensor.h"
#include <Alarm.h>
#include <door/DoorDebouncer.h>
#include "pins.h"

Alarm galitasAlarm;
ArduinoDoorSensor door_sensor(DOOR_SENSOR_PIN);
DoorDebouncer db(galitasAlarm, door_sensor);

void setup()
{
    const unsigned long now = millis();
    Serial.begin(115200);
    door_sensor.setup();
    db.begin(now);
}

void loop()
{
    const unsigned long now = millis();
    db.update(now);
    delay(1);
}