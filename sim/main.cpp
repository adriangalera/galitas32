// sim/main.cpp

#include "../lib/AlarmCore/src/Alarm.h"
#include "DummyDoorObserver.h"
#include "DummyDoorSensor.h"
#include "../lib/AlarmCore/src/DoorDebouncer.h"
#include <Logger.h>

#include <iostream>
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
    Alarm alarm;
    DummyDoorSensor door_sensor;
    DoorDebouncer db(door_sensor);
    db.setObserver(alarm);

    std::cout << "a = abrir, c = cerrar, q = salir\n";
    fflush(stdout);

    char command;
    DoorState state = DoorState::Unknown;

    bool running = true;

    while (running)
    {

        door_sensor.set(state);
        db.update(millis());
        
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
                    LOG_DEBUG("Opening the door");
                    state = DoorState::Open;
                }
                else if (command == 'c')
                {
                    LOG_DEBUG("Closing the door");
                    state = DoorState::Closed;
                }
            }

            if (input.revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                fprintf(stderr, "stdin desconectado o no disponible\n");
                break;
            }
        }
    }

    /*
    SimulatedHardware hardware;
    AlarmApp app(hardware);

    app.update();

    std::cout << "LED: " << colorName(hardware.currentLed) << '\n';


    char command;

    while (std::cin >> command) {
        if (command == 'q') {
            break;
        }

        if (command == 'a') {
            hardware.doorOpen = true;
        } else if (command == 'c') {
            hardware.doorOpen = false;
        } else {
            continue;
        }

        app.update();

        std::cout
            << "Puerta: " << (hardware.doorOpen ? "abierta" : "cerrada")
            << " | LED: " << colorName(hardware.currentLed)
            << '\n';
    }
    */
    return 0;
}