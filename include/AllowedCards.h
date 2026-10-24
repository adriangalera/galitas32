#pragma once
#include <utils/Logger.h>

#ifdef ARDUINO
    #include "AllowedCards.local.h"
#else
    #include "AllowedCards.example.h"
#endif