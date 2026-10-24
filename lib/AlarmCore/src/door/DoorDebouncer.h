/*
Debounces events coming from a door sensor and only emits changes via the observer
*/
#pragma once
#include <door/DoorSensor.h>
#include <door/DoorObserver.h>
#include <cstdint>

class DoorDebouncer
{
    public:
    explicit DoorDebouncer(
        DoorObserver &observer,
        DoorSensor &sensor,
        uint32_t debounceMs = 50)
        : observer_(observer), sensor_(sensor), debounceMs_(debounceMs) {}
        
        void begin(uint32_t now)
        {
            stable_ = sensor_.read();
            candidate_ = stable_;
            candidateSince_ = now;
            notify();
        }
        
        DoorState state() const
        {
            return stable_;
        }
        
        void update(uint32_t now)
        {
            const DoorState reading = sensor_.read();
            
            // Every time the reading changes, we reset the candidate and the timer.
            if (reading != candidate_)
            {
                candidate_ = reading;
                candidateSince_ = now;
            }
            
            //only accept a different state if it has been stable for the debounce period
            if (candidate_ != stable_ &&
                static_cast<uint32_t>(now - candidateSince_) >= debounceMs_)
                {
                    stable_ = candidate_;
                    notify();
                    
                    return;
                }
                
                return;
            }
            
            private:
            DoorObserver &observer_;
            
            DoorSensor &sensor_;
            const uint32_t debounceMs_;
            
            DoorState stable_ = DoorState::Unknown;
            DoorState candidate_ = DoorState::Unknown;
            uint32_t candidateSince_ = 0;
            
            void notify()
            {
                observer_.onDoorStateChanged(state());
            }
        };