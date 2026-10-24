#pragma once

#include <door/DoorObserver.h>
#include <card/ArmDisarmObserver.h>
#include <utils/Logger.h>
#include <led/LedController.h>

class Alarm final : public DoorObserver, public ArmDisarmObserver
{
public:
    explicit Alarm(LedController& ledController)
        : ledController_(ledController)
    {
    }

    void onDoorStateChanged(DoorState doorState) override
    {
        LOG_DEBUG("Received door state change: %s", toString(doorState));
        state_ = doorState;
        ledController_.setDoorOpen(doorState == DoorState::Open);
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
    LedController& ledController_;
    DoorState state_ = DoorState::Unknown;
};