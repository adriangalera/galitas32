/*
Contains the logic to manipulate the LED based on the system state.
There are three types of signals that can be active at the same time:
- Door signal: indicates that the door is open. It has the lowest priority  
- Confirmation signal: indicates that the system has been armed or disarmed. It has a higher priority than the door signal and is active for a limited time.
- Alert signal: indicates that the system is in an alert state. It has the highest priority

The algorithm selects the signal with the highest priority to be displayed on the LED. If no signal is active, the LED is turned off.

*/
#pragma once

#include <stdint.h>
#include <led/Led.h>

class LedController {
public:
    explicit LedController(Led& led)
        : led_(led) {}

    void setDoorOpen(bool open) {
        doorSignal_.active = open;
    }

    void showArming(uint32_t now) {
        showConfirmation(Pattern::Yellow, now);
    }

    void showDisarmed(uint32_t now) {
        showConfirmation(Pattern::Green, now);
    }

    void clearConfirmation() {
        confirmationSignal_.active = false;
    }

    void enableAlerting(uint32_t now) {
        if (alertSignal_.active) return;

        clearConfirmation();

        alertSignal_.startedAt = now;
        alertSignal_.active = true;
    }

    void disableAlerting() {
        alertSignal_.active = false;
    }

    void update(uint32_t now) {
        Request* requests[] = {
            &doorSignal_,
            &confirmationSignal_,
            &alertSignal_
        };

        const Request* selected = nullptr;

        for (Request* request : requests) {
            expire(*request, now);

            if (!request->active) continue;

            if (selected == nullptr ||
                request->priority > selected->priority) {
                selected = request;
            }
        }

        applyColor(resolveColor(selected, now));
    }

private:
    enum class Pattern {
        White,
        Green,
        Yellow,
        RedBlue
    };

    struct Request {
        Pattern pattern;
        uint8_t priority;
        uint32_t duration;
        uint32_t startedAt;
        bool active;

        Request(Pattern pattern, uint8_t priority, uint32_t duration)
            : pattern(pattern),
              priority(priority),
              duration(duration),
              startedAt(0),
              active(false) {}
    };

    static constexpr uint32_t ConfirmationDurationMs = 10000;
    static constexpr uint32_t AlertColorDurationMs = 250;

    static constexpr uint8_t DoorPriority = 10;
    static constexpr uint8_t ConfirmationPriority = 20;
    static constexpr uint8_t AlertPriority = 30;

    Led& led_;

    // duration == 0: active until explicitly cleared
    Request doorSignal_{
        Pattern::White, DoorPriority, 0
    };

    Request confirmationSignal_{
        Pattern::Green, ConfirmationPriority, ConfirmationDurationMs
    };

    // duration == 0: active until explicitly cleared
    Request alertSignal_{
        Pattern::RedBlue, AlertPriority, 0
    };

    LedColor lastColor_ = LedColor::Off;
    bool outputInitialized_ = false;

    void showConfirmation(Pattern pattern, uint32_t now) {
        if (alertSignal_.active) return;

        confirmationSignal_.pattern = pattern;
        confirmationSignal_.startedAt = now;
        confirmationSignal_.active = true;
    }

    static void expire(Request& request, uint32_t now) {
        if (!request.active || request.duration == 0) return;

        // support unsigned arithmetic to handle millis() rollover.
        const uint32_t elapsed = now - request.startedAt;

        if (elapsed >= request.duration) {
            request.active = false;
        }
    }

    static LedColor resolveColor(
        const Request* request,
        uint32_t now
    ) {
        if (request == nullptr) return LedColor::Off;

        switch (request->pattern) {
        case Pattern::White:
            return LedColor::White;

        case Pattern::Green:
            return LedColor::Green;

        case Pattern::Yellow:
            return LedColor::Yellow;

        case Pattern::RedBlue: {
            const uint32_t elapsed = now - request->startedAt;
            const bool redPhase =
                (elapsed / AlertColorDurationMs) % 2u == 0;

            return redPhase ? LedColor::Red : LedColor::Blue;
        }
        }

        return LedColor::Off;  // GCOVR_EXCL_LINE
    }

    void applyColor(LedColor color) {
        if (outputInitialized_ && color == lastColor_) return;

        led_.setColor(color);

        lastColor_ = color;
        outputInitialized_ = true;
    }
};