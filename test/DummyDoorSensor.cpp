#include "IDoorSensor.h"

// Dummy door sensor that just returns the value set
class DummyDoorSensor final : public IDoorSensor {
public:
    void set(DoorState doorState) {
        state_ = doorState;
    }

    DoorState read() const override {
        return state_;
    }

private:
    DoorState state_ = DoorState::Closed;
};