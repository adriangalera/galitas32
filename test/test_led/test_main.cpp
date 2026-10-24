#include <unity.h>
#include <cstdint>
#include <led/LedController.h>
#include <mocks/RecordingLed.h>



struct Fixture {
    RecordingLed led;
    LedController controller{led};
};

void setUp() {}
void tearDown() {}

static void assertColor(LedColor expected, const RecordingLed& led) {
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(expected),
        static_cast<int>(led.color_)
    );
}

// Unsigned addition intentionally wraps like millis().
static uint32_t after(uint32_t start, uint32_t elapsed) {
    return start + elapsed;
}

void test_first_update_explicitly_turns_led_off() {
    Fixture f;

    TEST_ASSERT_EQUAL_UINT32(0, f.led.writes_);

    f.controller.update(0);

    assertColor(LedColor::Off, f.led);
    TEST_ASSERT_EQUAL_UINT32(1, f.led.writes_);
}

void test_unchanged_color_does_not_write_again() {
    Fixture f;

    f.controller.update(0);
    f.controller.update(1);

    TEST_ASSERT_EQUAL_UINT32(1, f.led.writes_);

    f.controller.setDoorOpen(true);
    f.controller.update(2);
    f.controller.update(3);

    assertColor(LedColor::White, f.led);
    TEST_ASSERT_EQUAL_UINT32(2, f.led.writes_);
}

void test_door_signal_lasts_until_door_closes() {
    Fixture f;

    f.controller.setDoorOpen(true);
    f.controller.update(0);
    assertColor(LedColor::White, f.led);

    f.controller.update(60000);
    assertColor(LedColor::White, f.led);

    f.controller.setDoorOpen(false);
    f.controller.update(60001);
    assertColor(LedColor::Off, f.led);
}

void test_arming_is_yellow_for_exactly_ten_seconds() {
    Fixture f;

    f.controller.showArming(100);
    f.controller.update(100);
    assertColor(LedColor::Yellow, f.led);

    f.controller.update(10099);
    assertColor(LedColor::Yellow, f.led);

    f.controller.update(10100);
    assertColor(LedColor::Off, f.led);
}

void test_disarmed_is_green_for_exactly_ten_seconds() {
    Fixture f;

    f.controller.showDisarmed(100);
    f.controller.update(100);
    assertColor(LedColor::Green, f.led);

    f.controller.update(10099);
    assertColor(LedColor::Green, f.led);

    f.controller.update(10100);
    assertColor(LedColor::Off, f.led);
}

void test_confirmation_overrides_door_then_restores_white() {
    Fixture f;

    f.controller.setDoorOpen(true);
    f.controller.showArming(100);
    f.controller.update(100);
    assertColor(LedColor::Yellow, f.led);

    f.controller.update(10100);
    assertColor(LedColor::White, f.led);
}

void test_door_changes_do_not_interrupt_confirmation() {
    Fixture f;

    f.controller.showDisarmed(0);

    f.controller.setDoorOpen(true);
    f.controller.update(100);
    assertColor(LedColor::Green, f.led);

    f.controller.setDoorOpen(false);
    f.controller.update(200);
    assertColor(LedColor::Green, f.led);

    f.controller.update(10000);
    assertColor(LedColor::Off, f.led);
}

void test_new_confirmation_replaces_color_and_restarts_duration() {
    Fixture f;

    f.controller.showArming(0);
    f.controller.update(0);
    assertColor(LedColor::Yellow, f.led);

    f.controller.showDisarmed(5000);
    f.controller.update(5000);
    assertColor(LedColor::Green, f.led);

    // The original confirmation would have expired here.
    f.controller.update(10000);
    assertColor(LedColor::Green, f.led);

    f.controller.update(14999);
    assertColor(LedColor::Green, f.led);

    f.controller.update(15000);
    assertColor(LedColor::Off, f.led);
}

void test_repeated_same_confirmation_restarts_duration() {
    Fixture f;

    f.controller.showArming(0);
    f.controller.update(0);

    f.controller.showArming(9000);
    f.controller.update(10000);
    assertColor(LedColor::Yellow, f.led);

    f.controller.update(18999);
    assertColor(LedColor::Yellow, f.led);

    f.controller.update(19000);
    assertColor(LedColor::Off, f.led);
}

void test_clear_confirmation_restores_underlying_signal() {
    Fixture f;

    f.controller.setDoorOpen(true);
    f.controller.showDisarmed(0);
    f.controller.update(0);
    assertColor(LedColor::Green, f.led);

    f.controller.clearConfirmation();
    f.controller.update(1);
    assertColor(LedColor::White, f.led);

    f.controller.setDoorOpen(false);
    f.controller.showArming(2);
    f.controller.update(2);
    assertColor(LedColor::Yellow, f.led);

    f.controller.clearConfirmation();
    f.controller.update(3);
    assertColor(LedColor::Off, f.led);
}

void test_alert_alternates_every_250_ms_from_activation() {
    Fixture f;

    f.controller.enableAlerting(100);

    f.controller.update(100);
    assertColor(LedColor::Red, f.led);

    f.controller.update(349);
    assertColor(LedColor::Red, f.led);

    f.controller.update(350);
    assertColor(LedColor::Blue, f.led);

    f.controller.update(599);
    assertColor(LedColor::Blue, f.led);

    f.controller.update(600);
    assertColor(LedColor::Red, f.led);

    // It remains active and handles skipped updates.
    f.controller.update(60350);
    assertColor(LedColor::Blue, f.led);
}

void test_alert_overrides_door_and_cancels_confirmation() {
    Fixture f;

    f.controller.setDoorOpen(true);
    f.controller.showArming(0);
    f.controller.update(0);
    assertColor(LedColor::Yellow, f.led);

    f.controller.enableAlerting(100);
    f.controller.update(100);
    assertColor(LedColor::Red, f.led);

    // Confirmation would still be valid if it had only been hidden.
    f.controller.disableAlerting();
    f.controller.update(200);
    assertColor(LedColor::White, f.led);
}

void test_confirmations_requested_during_alert_are_ignored() {
    Fixture f;

    f.controller.enableAlerting(0);

    f.controller.showArming(10);
    f.controller.update(10);
    assertColor(LedColor::Red, f.led);

    f.controller.showDisarmed(250);
    f.controller.update(250);
    assertColor(LedColor::Blue, f.led);

    f.controller.disableAlerting();
    f.controller.update(300);
    assertColor(LedColor::Off, f.led);
}

void test_closing_door_during_alert_prevents_white_afterwards() {
    Fixture f;

    f.controller.setDoorOpen(true);
    f.controller.enableAlerting(0);

    f.controller.setDoorOpen(false);
    f.controller.update(250);
    assertColor(LedColor::Blue, f.led);

    f.controller.disableAlerting();
    f.controller.update(300);
    assertColor(LedColor::Off, f.led);
}

void test_enabling_active_alert_does_not_restart_blink() {
    Fixture f;

    f.controller.enableAlerting(100);
    f.controller.update(350);
    assertColor(LedColor::Blue, f.led);

    f.controller.enableAlerting(400);
    f.controller.update(400);
    assertColor(LedColor::Blue, f.led);

    f.controller.update(600);
    assertColor(LedColor::Red, f.led);
}

void test_reactivating_alert_starts_a_new_red_phase() {
    Fixture f;

    f.controller.enableAlerting(0);
    f.controller.update(250);
    assertColor(LedColor::Blue, f.led);

    f.controller.disableAlerting();
    f.controller.update(260);
    assertColor(LedColor::Off, f.led);

    f.controller.enableAlerting(300);
    f.controller.update(300);
    assertColor(LedColor::Red, f.led);

    f.controller.update(550);
    assertColor(LedColor::Blue, f.led);
}

void test_disarm_confirmation_works_after_stopping_alert() {
    Fixture f;

    f.controller.enableAlerting(0);
    f.controller.update(0);

    f.controller.disableAlerting();
    f.controller.showDisarmed(100);
    f.controller.update(100);
    assertColor(LedColor::Green, f.led);

    f.controller.update(10100);
    assertColor(LedColor::Off, f.led);
}

void test_confirmation_expiration_across_millis_rollover() {
    Fixture f;
    const uint32_t start = UINT32_MAX - 5000u;

    f.controller.showDisarmed(start);
    f.controller.update(start);
    assertColor(LedColor::Green, f.led);

    f.controller.update(after(start, 9999u));
    assertColor(LedColor::Green, f.led);

    f.controller.update(after(start, 10000u));
    assertColor(LedColor::Off, f.led);
}

void test_alert_blink_across_millis_rollover() {
    Fixture f;
    const uint32_t start = UINT32_MAX - 100u;

    f.controller.enableAlerting(start);
    f.controller.update(start);
    assertColor(LedColor::Red, f.led);

    f.controller.update(after(start, 249u));
    assertColor(LedColor::Red, f.led);

    f.controller.update(after(start, 250u));
    assertColor(LedColor::Blue, f.led);

    f.controller.update(after(start, 500u));
    assertColor(LedColor::Red, f.led);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_first_update_explicitly_turns_led_off);
    RUN_TEST(test_unchanged_color_does_not_write_again);
    RUN_TEST(test_door_signal_lasts_until_door_closes);
    RUN_TEST(test_arming_is_yellow_for_exactly_ten_seconds);
    RUN_TEST(test_disarmed_is_green_for_exactly_ten_seconds);
    RUN_TEST(test_confirmation_overrides_door_then_restores_white);
    RUN_TEST(test_door_changes_do_not_interrupt_confirmation);
    RUN_TEST(test_new_confirmation_replaces_color_and_restarts_duration);
    RUN_TEST(test_repeated_same_confirmation_restarts_duration);
    RUN_TEST(test_clear_confirmation_restores_underlying_signal);
    RUN_TEST(test_alert_alternates_every_250_ms_from_activation);
    RUN_TEST(test_alert_overrides_door_and_cancels_confirmation);
    RUN_TEST(test_confirmations_requested_during_alert_are_ignored);
    RUN_TEST(test_closing_door_during_alert_prevents_white_afterwards);
    RUN_TEST(test_enabling_active_alert_does_not_restart_blink);
    RUN_TEST(test_reactivating_alert_starts_a_new_red_phase);
    RUN_TEST(test_disarm_confirmation_works_after_stopping_alert);
    RUN_TEST(test_confirmation_expiration_across_millis_rollover);
    RUN_TEST(test_alert_blink_across_millis_rollover);

    return UNITY_END();
}