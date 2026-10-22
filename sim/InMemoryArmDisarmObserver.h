#pragma once
#include <ArmDisarmObserver.h>

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
    }

    void disarm()
    {
        last_ = ArmDisarmEvent::Disarm;
    }

    ArmDisarmEvent get()
    {
        return last_;
    }

private:
    ArmDisarmEvent last_ = ArmDisarmEvent::None;
};
