#include <unity.h>
#include <memory>
#include <mocks/InMemoryCardReader.h>
#include <mocks/InMemoryArmDisarmObserver.h>
#include <card/CardHoldDetector.h>

static const uint32_t HOLD_MS = 5000;

CardUid emptyCard;
static InMemoryCardReader reader;
static InMemoryArmDisarmObserver observer;
static std::unique_ptr<CardHoldDetector> detector;

void setUp()
{
    reader = InMemoryCardReader();
    observer = InMemoryArmDisarmObserver();
    detector = std::make_unique<CardHoldDetector>(observer, reader, HOLD_MS);
}
void tearDown() { detector.reset(); }

void test_no_event_without_card()
{
    detector->update(0);
    detector->update(10000);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::None, observer.get());
}

void test_no_event_while_card_is_held_before_hold_time()
{
    reader.set(true, emptyCard);
    detector->update(0);
    detector->update(HOLD_MS - 1);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::None, observer.get());
}

void test_short_hold_disarms()
{
    reader.set(true, emptyCard);
    detector->update(100);
    reader.set(false, emptyCard);
    detector->update(100 + HOLD_MS - 1);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Disarm, observer.get());
}

void test_arms_at_hold_time_without_release()
{
    reader.set(true, emptyCard);
    detector->update(100);
    detector->update(100 + HOLD_MS);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Arm, observer.get());
}

void test_release_after_arming_does_not_disarm()
{
    reader.set(true, emptyCard);
    detector->update(0);
    detector->update(HOLD_MS);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Arm, observer.get());

    observer = InMemoryArmDisarmObserver();
    reader.set(false, emptyCard);
    detector->update(HOLD_MS + 100);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::None, observer.get());
}

void test_arm_not_repeated_while_card_stays()
{
    reader.set(true, emptyCard);
    detector->update(0);
    detector->update(HOLD_MS);

    observer = InMemoryArmDisarmObserver();
    detector->update(HOLD_MS + 1000);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::None, observer.get());
}

void test_arms_across_millis_wraparound()
{
    reader.set(true, emptyCard);
    detector->update(UINT32_MAX - 100);
    detector->update(UINT32_MAX - 100 + HOLD_MS);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Arm, observer.get());
}

void test_second_hold_is_measured_from_its_own_start()
{
    reader.set(true, emptyCard);
    detector->update(0);
    detector->update(HOLD_MS);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Arm, observer.get());
    reader.set(false, emptyCard);
    detector->update(HOLD_MS + 10);

    reader.set(true, emptyCard);
    detector->update(20000);
    reader.set(false, emptyCard);
    detector->update(20100);
    TEST_ASSERT_EQUAL(ArmDisarmEvent::Disarm, observer.get());
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_no_event_without_card);
    RUN_TEST(test_no_event_while_card_is_held_before_hold_time);
    RUN_TEST(test_short_hold_disarms);
    RUN_TEST(test_arms_at_hold_time_without_release);
    RUN_TEST(test_release_after_arming_does_not_disarm);
    RUN_TEST(test_arm_not_repeated_while_card_stays);
    RUN_TEST(test_arms_across_millis_wraparound);
    RUN_TEST(test_second_hold_is_measured_from_its_own_start);
    return UNITY_END();
}
