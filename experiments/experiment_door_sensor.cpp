#include <Arduino.h>

int door_sensor_pin = 19;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("===== APPLICATION STARTED =====");
    /* An internal 20K-ohm resistor is pulled to 5V.
    This configuration causes the input to read HIGH when the switch is open, and LOW when it is closed.
    */
    pinMode(door_sensor_pin, INPUT_PULLUP);
    Serial.println("Setup complete ...");
}
int read()
{
    return digitalRead(door_sensor_pin) == LOW ? 0 : 1;
}
void loop()
{
    int state = read();
    if (state == 0)
    {
        Serial.println("Door is closed");
    }
    else
    {
        Serial.println("Door is open");
    }
    delay(1000);
}