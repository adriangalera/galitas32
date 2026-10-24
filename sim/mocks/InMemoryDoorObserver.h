#pragma once

#include <door/DoorObserver.h>
#include <utils/Logger.h>

class InMemoryDoorObserver final : public DoorObserver
{
public:
    void onDoorStateChanged(DoorState doorState) override
    {
        LOG_DEBUG("Received door state changed: %s", toString(doorState));
        state_ = doorState;
    }

    const DoorState get()
    {
        return state_;
    }

private:
    DoorState state_ = DoorState::Unknown;
};