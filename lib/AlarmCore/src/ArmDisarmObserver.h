/*
Interface that abstract the events for arming and disarming the alarm.
The events will come from RFID and Telegram, so they interface is unique and must be implemented in the state machine.
The state machine does not care where the command came from.
*/
#pragma once

class ArmDisarmObserver
{
public:
    virtual ~ArmDisarmObserver() = default;

    virtual void arm() = 0;
    virtual void disarm() = 0;

};