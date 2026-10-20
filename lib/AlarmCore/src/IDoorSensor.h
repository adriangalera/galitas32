#pragma once

enum class DoorState
{
    Closed,
    Open
};

class IDoorSensor
{
public:
    virtual ~IDoorSensor() = default;

    virtual DoorState read() const = 0;
};