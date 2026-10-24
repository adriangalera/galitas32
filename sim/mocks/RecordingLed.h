#pragma once

#include <led/Led.h>
#include <utils/Logger.h>
#include <stdint.h>

class RecordingLed final : public Led {
public:

    void setup() override {
        // Initialization code for Recording LED
    }

    void setColor(LedColor newColor) override {
        LOG_DEBUG("RecordingLed::setColor: %s", toString(newColor));
        color_ = newColor;
        ++writes_;
    }

    LedColor color_ = LedColor::Off;
    uint32_t writes_ = 0;
};