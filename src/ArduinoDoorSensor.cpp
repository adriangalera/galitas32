#include <Arduino.h>
#include "IDoorSensor.h"

class ArduinoDoorSensor final : public IDoorSensor {
public:
    explicit ArduinoDoorSensor(uint8_t pin)
        : pin_(pin) {}

    void begin() {
        pinMode(pin_, INPUT_PULLUP);
    }

    DoorState read() const override {
        return digitalRead(pin_) == LOW
            ? DoorState::Closed
            : DoorState::Open;
    }

private:
    uint8_t pin_;
};