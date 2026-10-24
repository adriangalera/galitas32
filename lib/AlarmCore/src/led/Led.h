/* 
Abstract the LED behaviour
*/
#pragma once

enum class LedColor {
    Off,
    White,
    Green,
    Yellow,
    Red,
    Blue
};

inline const char* toString(LedColor color) noexcept
{
    switch (color)
    {
        case LedColor::Off:
        return "Off";
        case LedColor::White:
        return "White";
        case LedColor::Green:
        return "Green";
        case LedColor::Yellow:
        return "Yellow";
        case LedColor::Red:
        return "Red";
        case LedColor::Blue:
        return "Blue";
    }
    
    return "Unknown";  // GCOVR_EXCL_LINE
}


class Led {
    public:
    virtual ~Led() = default;
    virtual void begin() = 0;
    virtual void setColor(LedColor color) = 0;
};