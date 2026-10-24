/*
Abstraction for a door sensor
*/
#pragma once

enum class DoorState
{
    Unknown,
    Closed,
    Open
};

inline const char* toString(DoorState state) noexcept
{
    switch (state)
    {
    case DoorState::Closed:
        return "Closed";
    case DoorState::Open:
        return "Open";
    }

    return "Unknown";
}

class DoorSensor
{
public:
    virtual ~DoorSensor() = default;

    virtual DoorState read() const = 0;
};