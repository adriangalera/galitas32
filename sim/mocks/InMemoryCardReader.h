#pragma once

#include <card/CardReader.h>
#include <utils/Logger.h>

class InMemoryCardReader final : public CardReader
{
public:
    bool read(CardUid &uid) override
    {

        if (lastUid_.size != 0)
        {
            uid = lastUid_;
        }

        return present_;
    }

    void set(bool present, const CardUid &uid)
    {
        present_ = present;
        lastUid_ = uid;
    }

private:
    bool present_ = false;
    CardUid lastUid_;
};