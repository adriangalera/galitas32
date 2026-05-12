#include "door.h"
#include <Arduino.h>

int DOOR_SENSOR_PIN = 19;

void setup_door_sensor()
{
    /* An internal 20K-ohm resistor is pulled to 5V.
    This configuration causes the input to read HIGH when the switch is open, and LOW when it is closed.
    */
    pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
}
DoorState read_door_sensor()
{
    return digitalRead(DOOR_SENSOR_PIN) == LOW ? DOOR_CLOSED : DOOR_OPEN;
}