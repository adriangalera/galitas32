#include <DoorObserver.h>
#include <Logger.h>

class Alarm final : public DoorObserver
{
public:
    void onDoorStateChanged(DoorState doorState)
    {
        LOG_DEBUG("Received door state change: %d",
                  static_cast<int>(doorState));
        state_ = doorState;
    }

private:
    DoorState state_ = DoorState::Unknown;
};