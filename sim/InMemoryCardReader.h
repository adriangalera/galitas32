#include <CardReader.h>
#include <Logger.h>

class InMemoryCardReader final : public CardReader
{
public:
    bool read(CardUid &uid)
    {
        return present_;
    }

    void set(bool present)
    {
        present_ = present;
    }

private:
    bool present_ = false;
};