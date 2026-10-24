/*
Reads a card and notifies the observer when the card is held for a certain amount of time.
Triggers arm() when the card is held for holdMs milliseconds, and disarm() when the card is released before that time.
Contains also the logic to check if the card is allowed or not.
*/

#pragma once

#include <cstdint>
#include <cstring>
#include <card/CardReader.h>
#include <card/ArmDisarmObserver.h>
#include <AllowedCards.h>
#include <utils/Logger.h>

class CardHoldDetector
{
public:
    explicit CardHoldDetector(
        ArmDisarmObserver &observer,
        CardReader &reader,
        uint32_t holdMs = 5000)
        : observer_(observer), reader_(reader), holdMs_(holdMs)
    {
    }

    void update(uint32_t now)
    {
        CardUid uid{};
        const bool present = reader_.read(uid);

        if (present)
        {
            if (!tracking_ || !sameCard(uid, trackedUid_))
            {
                // A different card interrupts the previous interaction.
                tracking_ = false;

                if (!isCardAllowed(uid))
                {
                    LOG_DEBUG("Card not allowed");
                    return;
                }

                trackedUid_ = uid;
                startedAt_ = now;
                tracking_ = true;
                holdReported_ = false;
            }

            if (!holdReported_ && heldLongEnough(now))
            {
                holdReported_ = true;
                observer_.arm();
            }

            return;
        }

        if (!tracking_)
        {
            return;
        }

        tracking_ = false;

        // The tracked card was authorized when tracking started.
        if (!holdReported_)
        {
            observer_.disarm();
        }
    }

private:
    ArmDisarmObserver &observer_;
    CardReader &reader_;
    const uint32_t holdMs_;

    CardUid trackedUid_{};
    uint32_t startedAt_ = 0;
    bool tracking_ = false;
    bool holdReported_ = false;

    bool heldLongEnough(uint32_t at) const
    {
        return static_cast<uint32_t>(at - startedAt_) >= holdMs_;
    }

    static bool sameCard(const CardUid &a, const CardUid &b)
    {
        return a.size > 0 &&
               a.size <= sizeof(a.bytes) &&
               a.size == b.size &&
               std::memcmp(a.bytes, b.bytes, a.size) == 0;
    }

    static bool isCardAllowed(const CardUid &card)
    {
        for (const CardUid &allowed : allowedCards)
        {
            if (sameCard(card, allowed))
            {
                return true;
            }
        }

        return false;
    }
};