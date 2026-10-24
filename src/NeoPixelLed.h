#pragma once
#include <Adafruit_NeoPixel.h>

#include <led/Led.h>
#include <utils/Logger.h>
#include <stdint.h>

class NeoPixelLed final : public Led {
    public:
    
    explicit NeoPixelLed(uint8_t pin, uint8_t ledCount)
    : pin_(pin), ledCount_(ledCount) {}
    
    void setup() override {
        LOG_DEBUG("NeoPixelLed::setup: pin=%d, ledCount=%d", pin_, ledCount_);
        strip_ = Adafruit_NeoPixel(ledCount_, pin_, NEO_GRB + NEO_KHZ800);
        strip_.begin();
        strip_.clear();
        strip_.show();
    }
    
    void setColor(LedColor newColor) override {
        if(newColor == LedColor::Off) {
            strip_.clear();
        } else {
            uint32_t colorValue = 0;
            switch (newColor) {
                case LedColor::White:
                colorValue = strip_.Color(255, 255, 255);
                break;
                case LedColor::Green:
                colorValue = strip_.Color(0, 255, 0);
                break;
                case LedColor::Yellow:
                colorValue = strip_.Color(255, 255, 0);
                break;
                case LedColor::Red:
                colorValue = strip_.Color(255, 0, 0);
                break;
                case LedColor::Blue:
                colorValue = strip_.Color(0, 0, 255);
                break;
                default:
                LOG_DEBUG("NeoPixelLed::setColor: Unknown color");
                return;
            }
            for (uint8_t i = 0; i < ledCount_; ++i) {
                strip_.setPixelColor(i, colorValue);
            }
        }
    }
    
    private:
    const uint8_t pin_;
    const uint8_t ledCount_;
    Adafruit_NeoPixel strip_;
};