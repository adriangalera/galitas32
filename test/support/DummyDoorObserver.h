#include <DoorObserver.h>

class DummyDoorObserver final : public DoorObserver
{
public:
    void onDoorStateChanged(DoorState doorState)
    {
        state_ = doorState;
    }

    DoorState get()
    {
        return state_;
    }

private:
    DoorState state_ = DoorState::Unknown;
};