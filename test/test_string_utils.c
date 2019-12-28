#include "string_utils.h"

#include "unity.h"

void setUp() { }

void tearDown() { }

void test_reverseString_will_NotModifyInput_when_GivenZeroLength() {
    char input[] = "foo";

    reverseString(input, 0);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("foo", input, 3);
}

void test_reverseString_will_ModifyInputWithItsReverse() {
    char input[] = "bar";

    reverseString(input, 3);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("rab", input, 3);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_reverseString_will_NotModifyInput_when_GivenZeroLength);
    RUN_TEST(test_reverseString_will_ModifyInputWithItsReverse);
    return UNITY_END();
}