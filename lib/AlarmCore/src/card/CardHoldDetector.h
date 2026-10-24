#pragma once

#include <cstdint>
#include "CardReader.h"
#include "ArmDisarmObserver.h"

class CardHoldDetector
{
public:
    explicit CardHoldDetector(ArmDisarmObserver &observer, CardReader &reader, uint32_t holdMs = 5000) // holdMs is the time needed to send the arm signal
            : observer_(observer), reader_(reader), holdMs_(holdMs) 
    {
    }

    void update(uint32_t now)
    {
        CardUid uid;
        const bool present = reader_.read(uid);

        if (present)
        {
            if (!tracking_)
            {
                tracking_ = true;
                startedAt_ = now;
                lastSeenAt_ = now;
            }

            // Report the hold as soon as the hold time is crossed, without waiting for the release
            const bool wasHeld = heldLongEnough(lastSeenAt_);
            lastSeenAt_ = now;

            if (!wasHeld && heldLongEnough(now))
            {
                observer_.arm();
            }

            return;
        }

        if (!tracking_)
        {
            return;
        }

        // The card is no longer present
        tracking_ = false;

        // TODO: Have a repository with the allowed cards and check the reading against it

        // A hold that reached the hold time was already reported; a shorter one is a tap.
        // Whether the alarm is armed or disarmed is up to the observer, not to this class.
        if (!heldLongEnough(lastSeenAt_))
        {
            observer_.disarm();
        }
    }

private:
    ArmDisarmObserver &observer_;
    CardReader &reader_;
    const uint32_t holdMs_;

    
    uint32_t startedAt_ = 0;
    uint32_t lastSeenAt_ = 0;
    bool tracking_ = false;

    bool heldLongEnough(uint32_t at) const
    {
        return static_cast<uint32_t>(at - startedAt_) >= holdMs_;
    }
};