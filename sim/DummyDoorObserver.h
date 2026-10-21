#include <DoorObserver.h>
#include <cstdio>

class DummyDoorObserver final : public DoorObserver
{
public:
    void onDoorStateChanged(DoorState doorState)
    {
        std::printf("Received door state changed: %d\n",
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