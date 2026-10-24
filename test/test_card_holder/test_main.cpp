#include <unity.h>
#include <memory>
#include <cstdint>
#include <mocks/InMemoryCardReader.h>
#include <mocks/InMemoryArmDisarmObserver.h>
#include <card/CardHoldDetector.h>

static constexpr uint32_t HOLD_MS = 5000;

static const CardUid ALLOWED_A = {{0x11, 0x22, 0x33, 0x44}, 4};
static const CardUid ALLOWED_B = {{0xAA, 0xBB, 0xCC, 0xDD}, 4};
static const CardUid DENIED_A = {{0x99}, 1};
static const CardUid DENIED_B = {{0x98}, 1};
static const CardUid EMPTY_UID{};

static InMemoryCardReader reader;
static InMemoryArmDisarmObserver observer;
static std::unique_ptr<CardHoldDetector> detector;

void setUp()
{
    reader = InMemoryCardReader();
    observer = InMemoryArmDisarmObserver();
    detector = std::make_unique<CardHoldDetector>(
        observer, reader, HOLD_MS);
}

void tearDown()
{
    detector.reset();
}

static void present(const CardUid &uid, uint32_t now)
{
    reader.set(true, uid);
    detector->update(now);
}

static void release(uint32_t now)
{
    // A failed read must not need to preserve the previous UID.
    reader.set(false, EMPTY_UID);
    detector->update(now);
}

static void expectEvent(ArmDisarmEvent expected)
{
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(expected),
        static_cast<int>(observer.get()));
}

static void clearEvent()
{
    // Assignment preserves the observer object referenced by the detector.
    observer = InMemoryArmDisarmObserver();
}

static void expectRejected(const CardUid &uid)
{
    present(uid, 0);
    expectEvent(ArmDisarmEvent::None);

    present(uid, HOLD_MS - 1);
    expectEvent(ArmDisarmEvent::None);

    present(uid, HOLD_MS);
    expectEvent(ArmDisarmEvent::None);

    present(uid, HOLD_MS * 2);
    expectEvent(ArmDisarmEvent::None);

    release(HOLD_MS * 2 + 1);
    expectEvent(ArmDisarmEvent::None);
}

void test_no_event_without_card()
{
    release(0);
    release(10000);
    expectEvent(ArmDisarmEvent::None);
}

void test_allowed_card_waits_until_threshold()
{
    present(ALLOWED_A, 100);
    expectEvent(ArmDisarmEvent::None);

    present(ALLOWED_A, 100 + HOLD_MS - 1);
    expectEvent(ArmDisarmEvent::None);

    present(ALLOWED_A, 100 + HOLD_MS);
    expectEvent(ArmDisarmEvent::Arm);
}

void test_allowed_tap_disarms_once()
{
    present(ALLOWED_A, 100);
    release(101);
    expectEvent(ArmDisarmEvent::Disarm);

    clearEvent();
    release(102);
    release(10000);
    expectEvent(ArmDisarmEvent::None);
}

void test_release_just_before_threshold_disarms()
{
    present(ALLOWED_A, 100);
    present(ALLOWED_A, 100 + HOLD_MS - 1);
    expectEvent(ArmDisarmEvent::None);

    release(100 + HOLD_MS - 1);
    expectEvent(ArmDisarmEvent::Disarm);
}

void test_hold_first_observed_after_threshold_arms()
{
    present(ALLOWED_A, 100);
    present(ALLOWED_A, 100 + HOLD_MS + 500);
    expectEvent(ArmDisarmEvent::Arm);
}

void test_arm_not_repeated_and_release_does_not_disarm()
{
    present(ALLOWED_A, 0);
    present(ALLOWED_A, HOLD_MS);
    expectEvent(ArmDisarmEvent::Arm);

    clearEvent();
    present(ALLOWED_A, HOLD_MS + 1);
    expectEvent(ArmDisarmEvent::None);

    present(ALLOWED_A, HOLD_MS * 3);
    expectEvent(ArmDisarmEvent::None);

    release(HOLD_MS * 3 + 1);
    release(HOLD_MS * 3 + 2);
    expectEvent(ArmDisarmEvent::None);
}

void test_second_allowed_card_can_tap_and_hold()
{
    present(ALLOWED_B, 0);
    release(10);
    expectEvent(ArmDisarmEvent::Disarm);

    clearEvent();
    present(ALLOWED_B, 100);
    present(ALLOWED_B, 100 + HOLD_MS);
    expectEvent(ArmDisarmEvent::Arm);
}

void test_denied_tap_has_no_event()
{
    present(DENIED_A, 0);
    release(1);
    expectEvent(ArmDisarmEvent::None);
}

void test_denied_hold_has_no_event()
{
    expectRejected(DENIED_A);
}

void test_empty_uid_is_rejected()
{
    expectRejected(EMPTY_UID);
}

void test_oversized_uid_is_rejected()
{
    CardUid invalid = ALLOWED_A;
    invalid.size = static_cast<uint8_t>(sizeof(invalid.bytes) + 1);

    expectRejected(invalid);
}

void test_matching_prefix_with_different_size_is_rejected()
{
    CardUid shorter = ALLOWED_A;
    shorter.size = 3;
    expectRejected(shorter);
}

void test_same_size_with_different_bytes_is_rejected()
{
    CardUid different = ALLOWED_A;
    different.bytes[3] = 0x45;
    expectRejected(different);
}

void test_unused_bytes_do_not_affect_authorization_or_tracking()
{
    present(ALLOWED_A, 0);

    CardUid sameUid = ALLOWED_A;
    sameUid.bytes[9] = 0xFF;

    present(sameUid, HOLD_MS);
    expectEvent(ArmDisarmEvent::Arm);
}

// Check every allowed/denied transition without an intervening release.
// Use different authorized cards so A -> B must restart the timer.
void test_card_switch_transition_matrix()
{
    const CardUid originals[] = {ALLOWED_A, DENIED_A};
    const CardUid replacements[] = {ALLOWED_B, DENIED_B};

    for (const CardUid &original : originals)
    {
        for (unsigned int i = 0; i < 2; ++i)
        {
            for (unsigned int longHold = 0; longHold < 2; ++longHold)
            {
                setUp();

                present(original, 0);
                expectEvent(ArmDisarmEvent::None);

                const uint32_t switchedAt = HOLD_MS - 1;
                present(replacements[i], switchedAt);
                expectEvent(ArmDisarmEvent::None);

                // The original card's deadline must not trigger an event.
                present(replacements[i], HOLD_MS);
                expectEvent(ArmDisarmEvent::None);

                present(replacements[i], switchedAt + HOLD_MS - 1);
                expectEvent(ArmDisarmEvent::None);

                const bool authorized = i == 0;

                if (longHold)
                {
                    present(replacements[i], switchedAt + HOLD_MS);
                    expectEvent(authorized
                        ? ArmDisarmEvent::Arm
                        : ArmDisarmEvent::None);

                    clearEvent();
                    release(switchedAt + HOLD_MS + 1);
                    expectEvent(ArmDisarmEvent::None);
                }
                else
                {
                    release(switchedAt + HOLD_MS - 1);
                    expectEvent(authorized
                        ? ArmDisarmEvent::Disarm
                        : ArmDisarmEvent::None);
                }
            }
        }
    }
}

void test_denied_card_cancels_tracking_before_original_returns()
{
    present(ALLOWED_A, 0);
    present(DENIED_A, HOLD_MS - 1);
    expectEvent(ArmDisarmEvent::None);

    // Returning to the same authorized card starts a new interaction.
    present(ALLOWED_A, HOLD_MS);
    expectEvent(ArmDisarmEvent::None);

    present(ALLOWED_A, HOLD_MS * 2 - 1);
    expectEvent(ArmDisarmEvent::None);

    present(ALLOWED_A, HOLD_MS * 2);
    expectEvent(ArmDisarmEvent::Arm);
}

void test_switch_after_arming_starts_a_new_interaction()
{
    const CardUid replacements[] = {ALLOWED_B, DENIED_A};

    for (unsigned int i = 0; i < 2; ++i)
    {
        for (unsigned int longHold = 0; longHold < 2; ++longHold)
        {
            setUp();

            present(ALLOWED_A, 0);
            present(ALLOWED_A, HOLD_MS);
            expectEvent(ArmDisarmEvent::Arm);
            clearEvent();

            const uint32_t switchedAt = HOLD_MS + 10;
            present(replacements[i], switchedAt);
            expectEvent(ArmDisarmEvent::None);

            const bool authorized = i == 0;

            if (longHold)
            {
                present(replacements[i], switchedAt + HOLD_MS - 1);
                expectEvent(ArmDisarmEvent::None);

                present(replacements[i], switchedAt + HOLD_MS);
                expectEvent(authorized
                    ? ArmDisarmEvent::Arm
                    : ArmDisarmEvent::None);

                clearEvent();
                release(switchedAt + HOLD_MS + 1);
                expectEvent(ArmDisarmEvent::None);
            }
            else
            {
                release(switchedAt + 1);
                expectEvent(authorized
                    ? ArmDisarmEvent::Disarm
                    : ArmDisarmEvent::None);
            }
        }
    }
}

void test_repeated_presentations_reset_timer_and_event_state()
{
    // Tap, hold, hold, tap using the same UID.
    const bool longHolds[] = {false, true, true, false};

    for (unsigned int i = 0; i < 4; ++i)
    {
        clearEvent();
        const uint32_t start = i * 20000;

        present(ALLOWED_A, start);
        expectEvent(ArmDisarmEvent::None);

        present(ALLOWED_A, start + HOLD_MS - 1);
        expectEvent(ArmDisarmEvent::None);

        if (longHolds[i])
        {
            present(ALLOWED_A, start + HOLD_MS);
            expectEvent(ArmDisarmEvent::Arm);

            clearEvent();
            release(start + HOLD_MS + 1);
            expectEvent(ArmDisarmEvent::None);
        }
        else
        {
            release(start + HOLD_MS - 1);
            expectEvent(ArmDisarmEvent::Disarm);
        }
    }
}

void test_arms_across_millis_wraparound()
{
    const uint32_t start = UINT32_MAX - 100;

    present(ALLOWED_A, start);
    present(ALLOWED_A,
            static_cast<uint32_t>(start + HOLD_MS - 1));
    expectEvent(ArmDisarmEvent::None);

    present(ALLOWED_A,
            static_cast<uint32_t>(start + HOLD_MS));
    expectEvent(ArmDisarmEvent::Arm);

    clearEvent();
    release(static_cast<uint32_t>(start + HOLD_MS + 1));
    expectEvent(ArmDisarmEvent::None);
}

void test_tap_across_millis_wraparound()
{
    present(ALLOWED_A, UINT32_MAX - 100);
    release(50);
    expectEvent(ArmDisarmEvent::Disarm);
}

void test_zero_hold_time_arms_only_authorized_cards()
{
    detector = std::make_unique<CardHoldDetector>(
        observer, reader, 0);

    present(DENIED_A, 0);
    expectEvent(ArmDisarmEvent::None);

    present(ALLOWED_A, 1);
    expectEvent(ArmDisarmEvent::Arm);

    clearEvent();
    present(ALLOWED_A, 2);
    release(3);
    expectEvent(ArmDisarmEvent::None);
}

void test_release_after_deadline_without_observed_hold_disarms()
{
    present(ALLOWED_A, 0);

    // No successful read was observed at or beyond the threshold.
    release(HOLD_MS + 100);
    expectEvent(ArmDisarmEvent::Disarm);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_no_event_without_card);
    RUN_TEST(test_allowed_card_waits_until_threshold);
    RUN_TEST(test_allowed_tap_disarms_once);
    RUN_TEST(test_release_just_before_threshold_disarms);
    RUN_TEST(test_hold_first_observed_after_threshold_arms);
    RUN_TEST(test_arm_not_repeated_and_release_does_not_disarm);
    RUN_TEST(test_second_allowed_card_can_tap_and_hold);
    RUN_TEST(test_denied_tap_has_no_event);
    RUN_TEST(test_denied_hold_has_no_event);
    RUN_TEST(test_empty_uid_is_rejected);
    RUN_TEST(test_oversized_uid_is_rejected);
    RUN_TEST(test_matching_prefix_with_different_size_is_rejected);
    RUN_TEST(test_same_size_with_different_bytes_is_rejected);
    RUN_TEST(test_unused_bytes_do_not_affect_authorization_or_tracking);
    RUN_TEST(test_card_switch_transition_matrix);
    RUN_TEST(test_denied_card_cancels_tracking_before_original_returns);
    RUN_TEST(test_switch_after_arming_starts_a_new_interaction);
    RUN_TEST(test_repeated_presentations_reset_timer_and_event_state);
    RUN_TEST(test_arms_across_millis_wraparound);
    RUN_TEST(test_tap_across_millis_wraparound);
    RUN_TEST(test_zero_hold_time_arms_only_authorized_cards);
    RUN_TEST(test_release_after_deadline_without_observed_hold_disarms);

    return UNITY_END();
}