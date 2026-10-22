#pragma once

#include <stdint.h>
#include "CardReader.h"
#include "ArmDisarmObserver.h"

class CardHoldDetector
{
public:
    explicit CardHoldDetector(CardReader &reader, uint32_t holdMs = 5000) // holdMs is the time needed to send the arm signal
        : holdMs_(holdMs), reader_(reader)
    {
    }

    void setObserver(ArmDisarmObserver &observer)
    {
        observer_ = &observer;
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
            }

            return;
        }

        if (!tracking_)
        {
            return;
        }

        // The card is no longer present
        tracking_ = false;
        // Measure the elapsed time from the detection of the card
        const uint32_t duration = now - startedAt_;

        // TODO: Have a repository with the allowed cards and check the reading against it

        // Notify based on the elapsed time
        if (observer_ != nullptr)
        {
            if (duration >= holdMs_)
            {
                observer_->arm();
            }
            else
            {
                observer_->disarm();
            }
        }
    }

private:
    CardReader &reader_;
    const uint32_t holdMs_;

    ArmDisarmObserver *observer_ = nullptr;
    uint32_t startedAt_ = 0;
    bool tracking_ = false;
};