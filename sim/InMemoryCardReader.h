#include <CardReader.h>
#include <Logger.h>

class InMemoryCardReader final : public CardReader
{
public:
    bool read(CardUid &uid)
    {

        if (lastUid_.size != 0)
        {
            uid = lastUid_;
        }

        return present_;
    }

    void set(bool present, CardUid &uid)
    {
        present_ = present;
        lastUid_ = uid;
    }

private:
    bool present_ = false;
    CardUid lastUid_;
};