#include <unity.h>
#include <DoorSensor.h>
#include <DummyDoorSensor.h>

static DummyDoorSensor sensor;

void setUp()
{
    sensor = DummyDoorSensor();
}
void tearDown() {}

void test_open_door()
{
    sensor.set(DoorState::Open);
    TEST_ASSERT_EQUAL(DoorState::Open, sensor.read());
}

void test_close_door()
{
    sensor.set(DoorState::Closed);
    TEST_ASSERT_EQUAL(DoorState::Closed, sensor.read());
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_open_door);
    RUN_TEST(test_close_door);
    return UNITY_END();
}
