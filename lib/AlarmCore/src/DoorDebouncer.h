/*
Debounces events coming from a door sensor and only emits changes
*/
#pragma once
#include "DoorSensor.h"
#include "DoorObserver.h"
#include <cstdint>
class DoorDebouncer
{
public:
    explicit DoorDebouncer(
        DoorSensor &sensor,
        uint32_t debounceMs = 50)
        : sensor_(sensor),
          debounceMs_(debounceMs) {}

    void begin(uint32_t now)
    {
        stable_ = sensor_.read();
        candidate_ = stable_;
        candidateSince_ = now;
    }

    DoorState state() const
    {
        return stable_;
    }

    bool update(uint32_t now)
    {
        const DoorState reading = sensor_.read();

        // Cada rebote reinicia el periodo de estabilidad.
        if (reading != candidate_)
        {
            candidate_ = reading;
            candidateSince_ = now;
        }

        // Solo aceptamos un estado distinto del ya confirmado.
        if (candidate_ != stable_ &&
            static_cast<uint32_t>(now - candidateSince_) >= debounceMs_)
        {
            stable_ = candidate_;
            notify();

            return true;
        }

        return false;
    }

    void setObserver(DoorObserver &observer)
    {
        observer_ = &observer;
    }

private:
    DoorObserver *observer_ = nullptr;

    DoorSensor &sensor_;
    const uint32_t debounceMs_;

    DoorState stable_ = DoorState::Closed;
    DoorState candidate_ = DoorState::Closed;
    uint32_t candidateSince_ = 0;

    void notify()
    {
        if (observer_ != nullptr)
        {
            observer_->onDoorStateChanged(state());
        }
    }
};