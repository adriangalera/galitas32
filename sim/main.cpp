#include "Alarm.h"
#include "InMemoryDoorObserver.h"
#include "InMemoryDoorSensor.h"
#include "InMemoryCardReader.h"
#include "CardHoldDetector.h"
#include "ArmDisarmObserver.h"
#include "DoorDebouncer.h"
#include "Logger.h"

#include <chrono>
#include <cstdint>
#include <poll.h>
#include <unistd.h>
#include <thread>

uint32_t millis()
{
    using Clock = std::chrono::steady_clock;
    static const auto start = Clock::now();

    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            Clock::now() - start)
            .count());
}

int main()
{
    CardUid card{{0xDE, 0xAD, 0xBE, 0xEF}, 4};
    CardUid emptyCard;
    Alarm alarm;
    InMemoryDoorSensor door_sensor;
    DoorDebouncer db(door_sensor);
    db.setObserver(alarm);
    InMemoryCardReader card_reader;
    CardHoldDetector holdDetector(card_reader);
    holdDetector.setObserver(alarm);

    LOG_DEBUG("a = abrir, c = cerrar, t=acercar tarjeta, r=retirar tarjeta, q = salir");

    char command;
    DoorState state = DoorState::Unknown;

    bool running = true;

    while (running)
    {
        door_sensor.set(state);
        db.update(millis());
        holdDetector.update(millis());

        pollfd input{STDIN_FILENO, POLLIN, 0};
        int result = poll(&input, 1, 0);

        if (result < 0)
        {
            perror("poll");
            break;
        }

        if (result > 0)
        {
            if (input.revents & POLLIN)
            {
                char command;
                ssize_t count = ::read(STDIN_FILENO, &command, 1);

                if (count < 0)
                {
                    perror("read");
                    break;
                }

                if (count == 0)
                {
                    fprintf(stderr, "stdin cerrado\n");
                    break;
                }

                if (command == 'q')
                    running = false;
                else if (command == 'a')
                {
                    LOG_DEBUG("Door is opened");
                    state = DoorState::Open;
                }
                else if (command == 'c')
                {
                    LOG_DEBUG("Door is closed");
                    state = DoorState::Closed;
                }
                else if (command == 't')
                {
                    LOG_DEBUG("RFID tag close");
                    card_reader.set(true, card);
                }
                else if (command == 'r')
                {
                    LOG_DEBUG("RFID tag not close");
                    card_reader.set(false, emptyCard);
                }
            }

            if (input.revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                fprintf(stderr, "stdin desconectado o no disponible\n");
                break;
            }
        }
    }

    return 0;
}