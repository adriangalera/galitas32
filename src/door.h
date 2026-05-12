#pragma once

enum DoorState{
    DOOR_CLOSED = 0,
    DOOR_OPEN = 1
};

void setup_door_sensor();
DoorState read_door_sensor();