# Code review: galitas32

Scope: `lib/`, `sim/`, `src/`, `test/` (about 700 lines). I read every file. I did not build or run anything, so each finding comes from reading the code.

## 1. Summary

The structure is good for a project this size. The domain logic (door debouncing, card-hold detection) sits in `lib/AlarmCore`, behind small interfaces (`DoorSensor`, `CardReader`, `DoorObserver`, `ArmDisarmObserver`). That lets you test it on your laptop (`native` env) and run it in a terminal simulator (`sim`) without the ESP32. The debouncer handles `millis()` wraparound correctly and has a test for it. Tests are named after behaviour.

Problems, most important first:

1. **The firmware target (`src/main.cpp`) almost certainly doesn't compile.** `db.begin()` is called without its required argument (finding A1).
2. **The RFID logic assumes a perfectly stable reader.** Real RFID readers flicker, and this code would treat a flicker as the user removing the card (A2).
3. **The door state is never announced at startup**, and the simulator starts in a wrong state (A3, A4).
4. Several smaller C++ hygiene issues (missing `override`, member initialisation order, raw pointers). These are cheap to fix and prevent whole classes of bugs.

Severity labels: **High** = wrong behaviour or build break. **Medium** = will bite you soon. **Low** = polish.

---

## 2. A short C++ primer for the terms used below

- **Interface / abstract class**: a class with `= 0` methods (`virtual bool read(CardUid&) = 0;`). It declares *what* something can do, and other classes provide the *how*. `CardReader` is an interface. `InMemoryCardReader` (simulator) and a future `Mfrc522CardReader` (hardware) are implementations.
- **`override`**: a keyword you write after a method in a derived class (`DoorState read() const override`). It tells the compiler "this must replace a virtual method of the base class". If you misspell the name or get the signature wrong (for example, forget `const`), the compiler gives an error. Without `override`, you silently create a *new, unrelated* method and the base-class one is never replaced.
- **Observer pattern**: object A holds a pointer to object B and calls B's methods when something happens. Here `DoorDebouncer` calls `observer_->onDoorStateChanged(...)`.
- **Dangling pointer / reference**: a pointer to an object that has already been destroyed. Using it is *undefined behaviour* (crash, garbage, or "works until it doesn't").
- **Member initialisation order**: C++ initialises class members in the order they are *declared in the class*, not the order written in the constructor's `: a_(x), b_(y)` list. If those orders differ, the compiler warns (`-Wreorder`), and if one member's initial value depends on another, you get a real bug.
- **`constexpr`**: a constant evaluated at compile time. It is the type-safe replacement for `#define NAME 12`.
- **Unsigned wraparound**: `uint32_t` arithmetic wraps modulo 2^32, so `now - start` is correct even after `millis()` overflows (about every 49.7 days), as long as both are `uint32_t`. Your code does this right.

---

## 3. Findings

### A. Correctness

#### A5. [Medium] `CardHoldDetector` ignores *which* card

The card UID is read into `uid` and then dropped (`// TODO` at line 46). Anything that triggers the reader will arm or disarm. This is known and marked as TODO, so I only flag the design consequences:

- A **card swap** during a hold (card A removed, card B placed within the same poll) looks like one continuous hold. Track the UID of the card you started with and restart the hold if it changes.
- `CardUid` has no `operator==`, so the "allowed cards" check you plan will need one. Add it now, comparing `size` and only the first `size` bytes (don't compare the whole 10-byte array, because unused bytes are junk in real readers):

```cpp
bool operator==(const CardUid& a, const CardUid& b) {
    return a.size == b.size && memcmp(a.bytes, b.bytes, a.size) == 0;
}
```
- `size` is never validated against the `bytes[10]` capacity. The hardware driver must clamp it (MFRC522 UIDs are 4, 7 or 10 bytes, so 10 is fine, but enforce it where the UID is written).

---

---