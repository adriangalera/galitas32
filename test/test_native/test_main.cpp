#include <unity.h>
#include "DummyDoorSensor.cpp"

void setUp() {}
void tearDown() {}

void test_open_door()
{
    DummyDoorSensor sensor;
    sensor.set(DoorState::Open);
    TEST_ASSERT_EQUAL(sensor.read(), DoorState::Open);
}

void test_close_door()
{
    DummyDoorSensor sensor;
    sensor.set(DoorState::Closed);
    TEST_ASSERT_EQUAL(sensor.read(), DoorState::Closed);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_open_door);
    RUN_TEST(test_close_door);

    return UNITY_END();
}