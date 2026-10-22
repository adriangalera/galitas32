#include <DoorObserver.h>
#include <Logger.h>

class InMemoryDoorObserver final : public DoorObserver
{
public:
    void onDoorStateChanged(DoorState doorState)
    {
        LOG_DEBUG("Received door state changed: %d\n",
                  static_cast<int>(doorState));
        state_ = doorState;
    }

    DoorState get()
    {
        return state_;
    }

private:
    DoorState state_ = DoorState::Unknown;
};