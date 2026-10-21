/*
Abstraction for a door sensor
*/
#pragma once

enum class DoorState
{
    Closed,
    Open
};

class DoorSensor
{
public:
    virtual ~DoorSensor() = default;

    virtual DoorState read() const = 0;
};