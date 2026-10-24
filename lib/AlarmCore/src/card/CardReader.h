#pragma once
#include <cstdint>

struct CardUid
{
    uint8_t bytes[10] = {};
    uint8_t size = 0;
};

class CardReader
{
public:
    virtual ~CardReader() = default;

    // Reads a card and returns true if a card was read, false otherwise.
    // cardUid is filled with the card's UID if a card was read.
    virtual bool read(CardUid &uid) = 0;
};