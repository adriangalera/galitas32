#pragma once

#include <card/ArmDisarmObserver.h>
#include <utils/Logger.h>

enum class ArmDisarmEvent
{
    None,
    Arm,
    Disarm
};

class InMemoryArmDisarmObserver : public ArmDisarmObserver
{
public:
    void arm() override
    {
        last_ = ArmDisarmEvent::Arm;
        LOG_DEBUG("Arming the alarm!");
    }

    void disarm() override
    {
        last_ = ArmDisarmEvent::Disarm;
        LOG_DEBUG("Disarming the alarm!");
    }

    const ArmDisarmEvent get()
    {
        return last_;
    }

private:
    ArmDisarmEvent last_ = ArmDisarmEvent::None;
};
