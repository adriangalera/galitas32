#include <unity.h>
#include <memory>
#include <InMemoryDoorSensor.h>
#include <InMemoryDoorObserver.h>
#include <DoorDebouncer.h>

static InMemoryDoorSensor sensor;
static std::unique_ptr<DoorDebouncer> debouncer;

void setUp()
{
    sensor = InMemoryDoorSensor();
    debouncer = std::make_unique<DoorDebouncer>(sensor, 50);
    debouncer->begin(0);
}
void tearDown() { debouncer.reset(); }

void test_no_event_before_debounce_time()
{
    sensor.set(DoorState::Open);
    TEST_ASSERT_FALSE(debouncer->update(10));
    TEST_ASSERT_FALSE(debouncer->update(59)); // 49 ms since change
    TEST_ASSERT_EQUAL(DoorState::Closed, debouncer->state());
}

void test_event_after_debounce_time()
{
    sensor.set(DoorState::Open);
    TEST_ASSERT_FALSE(debouncer->update(10));
    TEST_ASSERT_TRUE(debouncer->update(60));
    TEST_ASSERT_EQUAL(DoorState::Open, debouncer->state());
}

void test_event_not_repeated()
{
    sensor.set(DoorState::Open);
    debouncer->update(10);
    TEST_ASSERT_TRUE(debouncer->update(60));
    TEST_ASSERT_FALSE(debouncer->update(100));
}

void test_bounce_restarts_timer()
{
    sensor.set(DoorState::Open);
    debouncer->update(10);
    sensor.set(DoorState::Closed); // bounce back to the stable state
    TEST_ASSERT_FALSE(debouncer->update(40));
    sensor.set(DoorState::Open);
    TEST_ASSERT_FALSE(debouncer->update(50));
    TEST_ASSERT_FALSE(debouncer->update(99)); // 49 ms since last change
    TEST_ASSERT_TRUE(debouncer->update(100));
}

void test_millis_wraparound()
{
    debouncer->begin(UINT32_MAX - 10);
    sensor.set(DoorState::Open);
    TEST_ASSERT_FALSE(debouncer->update(UINT32_MAX - 5));
    TEST_ASSERT_TRUE(debouncer->update(UINT32_MAX - 5 + 50));
}

void test_debouncer_observer()
{
    InMemoryDoorObserver observer;
    debouncer->setObserver(observer);

    sensor.set(DoorState::Open);
    TEST_ASSERT_FALSE(debouncer->update(0));
    TEST_ASSERT_TRUE(observer.get() == DoorState::Unknown);

    TEST_ASSERT_TRUE(debouncer->update(60));
    TEST_ASSERT_TRUE(observer.get() == DoorState::Open);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_no_event_before_debounce_time);
    RUN_TEST(test_event_after_debounce_time);
    RUN_TEST(test_event_not_repeated);
    RUN_TEST(test_bounce_restarts_timer);
    RUN_TEST(test_millis_wraparound);
    RUN_TEST(test_debouncer_observer);
    return UNITY_END();
}
