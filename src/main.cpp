#include <Arduino.h>
#include "ArduinoDoorSensor.h"
#include <DoorDebouncer.h>
#include "pins.h"

ArduinoDoorSensor door_sensor(DOOR_SENSOR_PIN);
DoorDebouncer db(door_sensor);

void setup()
{
    Serial.begin(115200);
    door_sensor.setup();
}

void loop()
{
    const unsigned long now = millis();
    db.update(now);
    delay(10);
}