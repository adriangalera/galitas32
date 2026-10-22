#pragma once

#include <door/DoorObserver.h>
#include <card/ArmDisarmObserver.h>
#include <utils/Logger.h>

class Alarm final : public DoorObserver, public ArmDisarmObserver
{
public:
    void onDoorStateChanged(DoorState doorState) override
    {
        LOG_DEBUG("Received door state change: %s", toString(doorState));
        state_ = doorState;
    }

    void arm() override
    {
        LOG_DEBUG("Arming the alarm ...");
    }
    void disarm() override
    {
        LOG_DEBUG("Disarming the alarm ...");
    }

private:
    DoorState state_ = DoorState::Unknown;
};