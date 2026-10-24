#include <unity.h>
#include <memory>
#include <mocks/InMemoryDoorSensor.h>
#include <mocks/InMemoryDoorObserver.h>
#include <door/DoorDebouncer.h>

static InMemoryDoorSensor sensor;
static InMemoryDoorObserver observer;
static std::unique_ptr<DoorDebouncer> debouncer;

void setUp()
{
    observer = InMemoryDoorObserver();
    sensor = InMemoryDoorSensor();
    debouncer = std::make_unique<DoorDebouncer>(observer, sensor, 50);
    debouncer->begin(0);
}
void tearDown() { debouncer.reset(); }

void test_door_state_to_string(){
    TEST_ASSERT_EQUAL_STRING("Unknown", toString(DoorState::Unknown));
    TEST_ASSERT_EQUAL_STRING("Closed", toString(DoorState::Closed));    
    TEST_ASSERT_EQUAL_STRING("Open", toString(DoorState::Open));       
}

void test_no_event_before_debounce_time()
{
    sensor.set(DoorState::Open);
    debouncer->update(10);
    TEST_ASSERT_EQUAL(DoorState::Closed, debouncer->state());
    debouncer->update(59); // 49 ms since change
    TEST_ASSERT_EQUAL(DoorState::Closed, debouncer->state());
}

void test_event_after_debounce_time()
{
    sensor.set(DoorState::Open);
    debouncer->update(10);
    TEST_ASSERT_EQUAL(DoorState::Closed, debouncer->state());
    debouncer->update(60);
    TEST_ASSERT_EQUAL(DoorState::Open, debouncer->state());
}

void test_event_not_repeated()
{
    sensor.set(DoorState::Open);
    debouncer->update(10);
    
    debouncer->update(60);
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Open);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Open);
    
    debouncer->update(100);
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Open);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Open);
}

void test_bounce_restarts_timer()
{
    sensor.set(DoorState::Open);
    debouncer->update(10);
    
    sensor.set(DoorState::Closed); // Bounce back to the stable state
    debouncer->update(40);
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Closed);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Closed);
    
    sensor.set(DoorState::Open);
    debouncer->update(50);
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Closed);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Closed);
    
    debouncer->update(99); // 49 ms since last change
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Closed);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Closed);
    
    debouncer->update(100); // 50 ms since last change
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Open);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Open);
}

void test_millis_wraparound()
{
    debouncer->begin(UINT32_MAX - 10);
    sensor.set(DoorState::Open);
    
    debouncer->update(UINT32_MAX - 5);
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Closed);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Closed);
    
    debouncer->update(43); // 49 ms since change, across wraparound
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Closed);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Closed);
    
    debouncer->update(44); // 50 ms since change, across wraparound
    TEST_ASSERT_TRUE(debouncer->state() == DoorState::Open);
    TEST_ASSERT_TRUE(observer.get() == DoorState::Open);
}

void test_debouncer_observer()
{
    observer = InMemoryDoorObserver();
    debouncer = std::make_unique<DoorDebouncer>(observer, sensor, 50);
    
    TEST_ASSERT_TRUE_MESSAGE(
        observer.get() == DoorState::Unknown,
        "Observer should be unknown before begin()");
        
        debouncer->begin(0);
        TEST_ASSERT_TRUE_MESSAGE(
            observer.get() == DoorState::Closed,
            "begin() should announce the initial state");
            
            sensor.set(DoorState::Open);
            debouncer->update(0);
            TEST_ASSERT_TRUE_MESSAGE(
                observer.get() == DoorState::Closed,
                "Observer should remain closed during debounce");
                
                debouncer->update(51);
                TEST_ASSERT_TRUE_MESSAGE(
                    observer.get() == DoorState::Open,
                    "Observer should report open after debounce");
                    
                    sensor.set(DoorState::Closed);
                    debouncer->update(60);
                    TEST_ASSERT_TRUE_MESSAGE(
                        observer.get() == DoorState::Open,
                        "Observer should remain open during debounce");
                        
                        debouncer->update(111);
                        TEST_ASSERT_TRUE_MESSAGE(
                            observer.get() == DoorState::Closed,
                            "Observer should report closed after debounce");
                        }
                        
                        void test_begin_announces_initial_open_state()
                        {
                            observer = InMemoryDoorObserver();
                            sensor.set(DoorState::Open);
                            debouncer = std::make_unique<DoorDebouncer>(observer, sensor, 50);
                            TEST_ASSERT_TRUE(debouncer->state() == DoorState::Unknown);
                            
                            debouncer->begin(0);
                            
                            TEST_ASSERT_TRUE_MESSAGE(observer.get() == DoorState::Open, "Door open at boot should be announced");
                            TEST_ASSERT_TRUE(debouncer->state() == DoorState::Open);
                        }
                        
                        
                        
                        int main()
                        {
                            UNITY_BEGIN();
                            
                            RUN_TEST(test_door_state_to_string);
                            
                            RUN_TEST(test_no_event_before_debounce_time);
                            RUN_TEST(test_event_after_debounce_time);
                            RUN_TEST(test_event_not_repeated);
                            RUN_TEST(test_bounce_restarts_timer);
                            RUN_TEST(test_millis_wraparound);
                            RUN_TEST(test_debouncer_observer);
                            RUN_TEST(test_begin_announces_initial_open_state);
                            return UNITY_END();
                        }
                        