/*
Interface that defines an observer of the door events
The state machine will implement this interface and receive the door state changes.
*/
#pragma once

#include <door/DoorSensor.h>

class DoorObserver
{
public:
    virtual ~DoorObserver() = default;

    virtual void onDoorStateChanged(DoorState state) = 0;
};