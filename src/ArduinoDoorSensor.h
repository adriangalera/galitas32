#pragma once

#include <door/DoorSensor.h>
#include <Arduino.h>

class ArduinoDoorSensor final : public DoorSensor
{
public:
    explicit ArduinoDoorSensor(uint8_t pin)
        : pin_(pin) {}

    void setup()
    {
        pinMode(pin_, INPUT_PULLUP);
    }

    DoorState read() const override
    {
        return digitalRead(pin_) == LOW
                   ? DoorState::Closed
                   : DoorState::Open;
    }

private:
    const uint8_t pin_;
};