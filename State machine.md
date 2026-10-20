# Alarm state machine

This document describes the alarm states, transitions, signals, and recovery behavior.

## Configuration

| Parameter | Value |
|---|---|
| Exit delay | 30 seconds |
| Entry delay | 10 seconds |
| Telegram alert reminder interval | 5 seconds |
| LED command confirmation duration | 10 seconds |
| Exit delay buzzer interval | 10 seconds |
| Entry delay buzzer interval | 1 second |

## General rules

### Events and door status

The controller tracks the door status independently of the alarm state:

- OPEN
- CLOSED
- UNKNOWN, until a valid sensor reading is available.

Door open and Door close represent confirmed changes in door status,
not repeated sensor readings. Sensor input must be debounced before
generating these events.

Arm and Disarm represent authorized commands from RFID or Telegram.

### Timers

- Start the exit timer when entering EXIT_DELAY.
- Start the entry timer when entering ENTRY_DELAY.
- Repeated commands and door events do not restart either timer.
- Cancel a state's timer when leaving that state.
- Ignore timeout events from abandoned states or earlier activations
  of the same state.
- Timers are not persisted across reboots.

Events are processed sequentially. An authorized Disarm command always
leads to IDLE, including when an alert timeout has just been processed.

### LED priority

LED indications use the following priority, from highest to lowest:

1. Red and blue blinking while in ALERTING.
2. Command confirmation: green or yellow for 10 seconds.
3. White service light while the door is open.
4. Off.

When a confirmation ends, restore the indication appropriate for the
current state and door status.

Closing the door removes the white service light but does not interrupt
an active command confirmation.

Entering ALERTING cancels any previous command confirmation.

### Telegram notifications

- Send one informative message per confirmed door opening in IDLE,
  EXIT_DELAY, or ENTRY_DELAY.
- Also send one informative message for the opening that causes
  CHECKING to transition to ENTRY_DELAY.
- Send an immediate alert when entering ALERTING.
- Send alert reminders every 5 seconds while in ALERTING.
- Stop reminders and cancel unsent reminders when leaving ALERTING.
- Telegram or network failures must not block local signals, timers,
  or state transitions.
- Discard obsolete queued notifications rather than delivering them
  as if they described the current state.

### Entry and exit actions

Event actions may execute without a state change.

State entry actions execute when entering a different state.
Events that leave the state unchanged do not repeat entry actions.

Startup recovery initializes the signals and timers required by the
restored state, as described in the recovery section.

When leaving EXIT_DELAY or ENTRY_DELAY:
- Cancel the corresponding timer.
- Stop its periodic buzzer sounds.

When leaving ALERTING:
- Stop the continuous buzzer.
- Stop the red and blue blinking.
- Stop Telegram alert reminders.

## IDLE

The alarm is disarmed.

An open door activates the white service light unless a higher-priority
LED indication is active.

### Entry actions

- Apply the LED indication corresponding to the current door status.
- If entered because of Disarm, show green for 10 seconds and send an
  "Alarm disarmed" confirmation.
- If entered because arming failed, emit a distinctive local warning
  and send a Telegram message explaining why arming failed.

### Transitions

| Event | Next state | Additional action |
|---|---|---|
| Door close | IDLE | Remove the white service light. |
| Door open | IDLE | Apply the white service light and send an informative Telegram message. |
| Disarm | IDLE | Show green for 10 seconds and confirm that the alarm is already disarmed. |
| Arm | EXIT_DELAY | |

## EXIT_DELAY

The alarm is preparing to arm and gives the user 30 seconds to leave
and close the door.

### Entry actions

- Start the 30-second exit timer.
- Show yellow for 10 seconds.
- Send an "Arming" notification. Do not confirm that the alarm is armed yet.
- Emit mild buzzer sounds every 10 seconds while this state remains active.
- Apply the service-light rule using the current door status, even if
  the door was already open before arming.

### Transitions

| Event / condition | Next state | Additional action |
|---|---|---|
| Door close | EXIT_DELAY | Remove the white service light. Do not restart the timer. |
| Door open | EXIT_DELAY | Apply the white service light and send an informative Telegram message. Do not restart the timer. |
| Disarm | IDLE | |
| Arm | EXIT_DELAY | Indicate that arming is already in progress. Do not restart the timer. |
| Exit timeout expires and door is closed | CHECKING | |
| Exit timeout expires and door is open | IDLE | Report "Arming cancelled: door is open" through a local warning and Telegram. |
| Exit timeout expires and door status is unknown | IDLE | Report "Arming cancelled: door status is unavailable" through a local warning and Telegram. |

Failure notifications in this table are the IDLE entry warning,
not additional duplicate notifications.

The system does not wait indefinitely for the door to close.

## CHECKING

The alarm is armed and monitors the door.

### Entry actions

When entered after a successful exit delay:
- Send an "Alarm armed" confirmation.
- Emit a brief local armed confirmation.
- Clear any remaining yellow arming indication.

### Transitions

| Event | Next state | Additional action |
|---|---|---|
| Door close | CHECKING | |
| Door open | ENTRY_DELAY | Apply the white service light and send one informative Telegram message. |
| Disarm | IDLE | |
| Arm | CHECKING | Confirm that the alarm is already armed. |

Disarming is allowed without opening the door first.

## ENTRY_DELAY

An opening has been detected. The user has 10 seconds to disarm before
the alarm starts alerting.

### Entry actions

- Start the 10-second entry timer.
- Emit a mild buzzer sound immediately, then every second while this
  state remains active.

### Transitions

| Event | Next state | Additional action |
|---|---|---|
| Door close | ENTRY_DELAY | Remove the white service light. Do not cancel or restart the timer. |
| Door open | ENTRY_DELAY | Apply the white service light and send an informative Telegram message. Do not restart the timer. |
| Disarm | IDLE | |
| Arm | ENTRY_DELAY | No effect. Do not restart the timer. |
| Entry timeout expires | ALERTING | |

During normal operation, closing the door does not cancel the detected
opening. Only Disarm prevents escalation to ALERTING.

Reboot recovery is an explicit exception, described below.

## ALERTING

An intrusion has been detected.

### Entry actions

- Activate the buzzer continuously at its configured alarm level.
- Blink the LEDs red and blue.
- Send an immediate Telegram alert.
- Send Telegram alert reminders every 5 seconds.

### Transitions

| Event | Next state | Additional action |
|---|---|---|
| Door close | ALERTING | Continue alerting. |
| Door open | ALERTING | Continue alerting. |
| Disarm | IDLE | |
| Arm | ALERTING | No effect. |

Closing the door does not stop the alarm.

## Startup, persistence, and recovery

Persist the alarm state whenever it changes. Timers are not persisted.

On reboot, restore the state according to this table:

| Persisted state | Restored state |
|---|---|
| IDLE | IDLE |
| EXIT_DELAY | IDLE |
| CHECKING | CHECKING |
| ENTRY_DELAY | CHECKING |
| ALERTING | ALERTING |

Persist the recovered state if it differs from the saved state.

Read the current door status from the sensor rather than restoring it
from storage. Until a valid reading is available, its status is UNKNOWN;
do not assume the door is closed.

### Restoring IDLE

- Restore the service light according to the current door status.
- Do not show a green command confirmation solely because of a reboot.
- If the saved state was EXIT_DELAY, emit a local warning and send:
  "Arming cancelled: system restarted. Alarm is disarmed."

### Restoring CHECKING

After obtaining a valid door reading:
- If the door is closed, remain in CHECKING.
- If the door is open, transition to ENTRY_DELAY, start a new 10-second
  entry timer, and send one informative Telegram message.

This policy intentionally discards an interrupted entry delay:
- If the door has closed before recovery, the alarm resumes CHECKING.
- If the door is still open, a fresh entry delay begins.

### Restoring ALERTING

- Resume the continuous buzzer and red and blue blinking immediately.
- Send an immediate Telegram alert indicating that alerting resumed
  after a restart.
- Resume Telegram reminders every 5 seconds.
- Do not wait for a door reading to resume alerting.

### First startup or invalid saved state

The initial-state policy when no valid saved state exists remains
to be defined.

## Sensor fault handling

The current hardware uses a digital door sensor with INPUT_PULLUP.

An open door and a disconnected sensor wire cannot be distinguished
by the current hardware. Both conditions are treated as an open door
and follow the normal state-machine rules.

UNKNOWN represents the absence of a valid reading, not a detected
disconnected wire.

Dedicated cable supervision and sensor fault detection are outside
the scope of the current hardware design.