#include <Alarm.h>
#include "mocks/InMemoryDoorSensor.h"
#include "mocks/InMemoryCardReader.h"
#include "card/CardHoldDetector.h"
#include "door/DoorDebouncer.h"
#include "utils/Logger.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>

#ifdef _WIN32
#include <conio.h>
#else
#include <cerrno>
#include <poll.h>
#include <unistd.h>
#endif

namespace
{
    constexpr int NoInput = -1;
    constexpr int InputClosed = -2;
    
    // Returns a character, NoInput, or InputClosed.
    int readCommand()
    {
        #ifdef _WIN32
        if (!_kbhit())
        return NoInput;
        
        const int command = _getch();
        
        // Consume special keys such as arrows and function keys.
        if (command == 0 || command == 0xE0)
        {
            (void)_getch();
            return NoInput;
        }
        
        return command;
        #else
        pollfd input{};
        input.fd = STDIN_FILENO;
        input.events = POLLIN;
        
        const int result = ::poll(&input, 1, 0);
        
        if (result < 0)
        {
            if (errno == EINTR)
            return NoInput;
            
            std::perror("poll");
            return InputClosed;
        }
        
        if (result == 0)
        return NoInput;
        
        // Read pending input before handling a possible hangup.
        if (input.revents & POLLIN)
        {
            unsigned char command{};
            const ssize_t count = ::read(STDIN_FILENO, &command, 1);
            
            if (count > 0)
            return static_cast<int>(command);
            
            if (count == 0)
            return InputClosed;
            
            if (errno == EINTR || errno == EAGAIN)
            return NoInput;
            
            std::perror("read");
            return InputClosed;
        }
        
        if (input.revents & (POLLERR | POLLHUP | POLLNVAL))
        return InputClosed;
        
        return NoInput;
        #endif
    }
} // namespace

std::uint32_t millis()
{
    using Clock = std::chrono::steady_clock;
    static const auto start = Clock::now();
    
    return static_cast<std::uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            Clock::now() - start)
            .count());
        }
        
        int main()
        {
            CardUid card{{0xAA, 0xBB, 0xCC, 0xDD}, 4};
            CardUid invalidCard{{0xCA, 0xFE, 0xBA, 0xBE}, 4};
            CardUid emptyCard{};
            
            Alarm alarm;
            InMemoryDoorSensor doorSensor;
            InMemoryCardReader cardReader;
            
            DoorDebouncer debouncer(alarm, doorSensor);
            CardHoldDetector holdDetector(alarm, cardReader);
            
            LOG_DEBUG(
                "\n"
                "o = open door\n"
                "c = close door\n"
                "a = present card 1\n"
                "s = remove card 1\n"
                "d = present card 2\n"
                "f = remove card 2\n"
                "q = quit");
                
                #ifndef _WIN32
                LOG_DEBUG("Press Enter after each command.");
                #endif
                
                doorSensor.set(DoorState::Closed);
                cardReader.set(false, emptyCard);
                debouncer.begin(millis());
                
                bool running = true;
                
                while (running)
                {
                    const int command = readCommand();
                    
                    switch (command)
                    {
                        case InputClosed:
                        LOG_DEBUG("Input closed or unavailable");
                        running = false;
                        break;
                        
                        case 'q':
                        running = false;
                        break;
                        
                        case 'o':
                        LOG_DEBUG("Sent door open");
                        doorSensor.set(DoorState::Open);
                        break;
                        
                        case 'c':
                        LOG_DEBUG("Sent door close");
                        doorSensor.set(DoorState::Closed);
                        break;
                        
                        case 'a':
                        LOG_DEBUG("RFID tag 1 present");
                        cardReader.set(true, card);
                        break;
                        
                        case 's':
                        LOG_DEBUG("RFID tag 1 removed");
                        cardReader.set(false, emptyCard);
                        break;
                        
                        case 'd':
                        LOG_DEBUG("RFID tag 2 present");
                        cardReader.set(true, invalidCard);
                        break;
                        
                        case 'f':
                        LOG_DEBUG("RFID tag 2 removed");
                        cardReader.set(false, emptyCard);
                        break;
                        
                        default:
                        // Ignore NoInput, newlines, and unrecognized keys.
                        break;
                    }
                    
                    if (!running)
                    break;
                    
                    const std::uint32_t now = millis();
                    debouncer.update(now);
                    holdDetector.update(now);
                    
                    // Avoid busy-waiting while keeping the simulation responsive.
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
                
                return 0;
            }