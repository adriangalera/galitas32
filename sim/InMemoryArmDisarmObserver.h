#pragma once
#include <ArmDisarmObserver.h>
#include <Logger.h>

enum class ArmDisarmEvent
{
    None,
    Arm,
    Disarm
};

class InMemoryArmDisarmObserver : public ArmDisarmObserver
{
public:
    void arm()
    {
        last_ = ArmDisarmEvent::Arm;
        LOG_DEBUG("Arming the alarm!");
    }

    void disarm()
    {
        last_ = ArmDisarmEvent::Disarm;
        LOG_DEBUG("Disarming the alarm!");
    }

    ArmDisarmEvent get()
    {
        return last_;
    }

private:
    ArmDisarmEvent last_ = ArmDisarmEvent::None;
};
