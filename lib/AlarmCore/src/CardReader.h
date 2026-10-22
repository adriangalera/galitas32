#pragma once
#include <stdint.h>

struct CardUid
{
    uint8_t bytes[10] = {};
    uint8_t size = 0;
};

class CardReader
{
public:
    virtual ~CardReader() = default;

    // true: se ha leído una tarjeta y uid contiene su identificador.
    // false: no hay una lectura disponible; uid no se modifica.
    virtual bool read(CardUid &uid) = 0;
};