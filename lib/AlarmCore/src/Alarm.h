#pragma once

#include <DoorObserver.h>
#include <ArmDisarmObserver.h>
#include <Logger.h>

class Alarm final : public DoorObserver, public ArmDisarmObserver
{
public:
    void onDoorStateChanged(DoorState doorState)
    {
        LOG_DEBUG("Received door state change: %d",
                  static_cast<int>(doorState));
        state_ = doorState;
    }

    void arm()
    {
        LOG_DEBUG("Arming the alarm ...");
    }
    void disarm()
    {
        LOG_DEBUG("Disarming the alarm ...");
    }

private:
    DoorState state_ = DoorState::Unknown;
};