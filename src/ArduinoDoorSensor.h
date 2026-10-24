#pragma once

#include <door/DoorSensor.h>
#include <Arduino.h>
#include <utils/Logger.h>

class ArduinoDoorSensor final : public DoorSensor
{
public:
    explicit ArduinoDoorSensor(uint8_t pin)
        : pin_(pin) {}

    void setup()
    {
        LOG_DEBUG("ArduinoDoorSensor::setup: pin=%d", pin_);
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