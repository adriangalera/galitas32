#include <unity.h>
#include <memory>
#include <InMemoryCardReader.h>
#include <InMemoryArmDisarmObserver.h>
#include <CardHoldDetector.h>

static const uint32_t HOLD_MS = 5000;

static InMemoryCardReader reader;
static InMemoryArmDisarmObserver observer;
static std::unique_ptr<CardHoldDetector> detector;

void setUp()
{
    reader = InMemoryCardReader();
    observer = InMemoryArmDisarmObserver();
    detector = std::make_unique<CardHoldDetector>(reader, HOLD_MS);
    detector->setObserver(observer);
}
void tearDown() { detector.reset(); }

void test_no_event_without_card()
{
    detector->update(0);
    detector->update(10000);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::None, observer.get());
}

void test_no_event_while_card_is_held()
{
    reader.set(true);
    detector->update(0);
    detector->update(10000);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::None, observer.get());
}

void test_short_hold_disarms()
{
    reader.set(true);
    detector->update(100);
    reader.set(false);
    detector->update(100 + HOLD_MS - 1);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Disarm, observer.get());
}

void test_long_hold_arms()
{
    reader.set(true);
    detector->update(100);
    reader.set(false);
    detector->update(100 + HOLD_MS);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Arm, observer.get());
}

void test_event_not_repeated_after_release()
{
    reader.set(true);
    detector->update(0);
    reader.set(false);
    detector->update(HOLD_MS);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Arm, observer.get());

    observer = InMemoryArmDisarmObserver();
    detector->update(HOLD_MS + 100);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::None, observer.get());
}

void test_second_hold_is_measured_from_its_own_start()
{
    reader.set(true);
    detector->update(0);
    reader.set(false);
    detector->update(HOLD_MS);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Arm, observer.get());

    reader.set(true);
    detector->update(20000);
    reader.set(false);
    detector->update(20100);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Disarm, observer.get());
}

void test_no_observer_does_not_crash()
{
    CardHoldDetector detectorWithoutObserver(reader, HOLD_MS);

    reader.set(true);
    detectorWithoutObserver.update(0);
    reader.set(false);
    detectorWithoutObserver.update(HOLD_MS);
    TEST_PASS();
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_no_event_without_card);
    RUN_TEST(test_no_event_while_card_is_held);
    RUN_TEST(test_short_hold_disarms);
    RUN_TEST(test_long_hold_arms);
    RUN_TEST(test_event_not_repeated_after_release);
    RUN_TEST(test_second_hold_is_measured_from_its_own_start);
    RUN_TEST(test_no_observer_does_not_crash);
    return UNITY_END();
}
