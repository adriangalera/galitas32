#include <unity.h>

void setUp() {}
void tearDown() {}

void test_setup_unity() {

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(0),
        static_cast<int>(0)
    );
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_setup_unity);

    return UNITY_END();
}