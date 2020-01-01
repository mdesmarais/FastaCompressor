#include "string_utils.h"

#include "unity.h"

#include <string.h>

static char g_buffer[255];

void setUp() {
    memset(g_buffer, 0, 255);
}

void tearDown() { }

void test_pathExtension_Should_ReturnSamePath_When_GivenZeroExtLength() {
    TEST_ASSERT_EQUAL(3, pathExtension("foo", 3, "txt", 0, g_buffer, 255));
    TEST_ASSERT_EQUAL_STRING("foo", g_buffer);
}

void test_pathExtension_Should_ReturnFilenameWithExt_When_GivenFilenameWithoutExt() {
    char expected[] = "foo.txt";
    TEST_ASSERT_EQUAL(strlen(expected), pathExtension("foo", 3, "txt", 3, g_buffer, 255));

    TEST_ASSERT_EQUAL_STRING(expected, g_buffer);
}

void test_pathExtension_Should_ReturSamePath_When_GivenPathWithoutFilename() {
    TEST_ASSERT_EQUAL(5, pathExtension("/foo/", 5, "txt", 3, g_buffer, 255));
    TEST_ASSERT_EQUAL_STRING("/foo/", g_buffer);
}

void test_pathExtension_Should_ReturnPathWithExt_When_GivenPathToFileWithoutExt() {
    char expected[] = "/foo/bar.txt";

    TEST_ASSERT_EQUAL(strlen(expected), pathExtension("/foo/bar", 8, "txt", 3, g_buffer, 255));
    TEST_ASSERT_EQUAL_STRING(expected, g_buffer);
}

void test_pathExtension_Should_ReturnPathWithModifiedExt_When_GivenPathToFileWithExt() {
    char expected[] = "/foo/bar.tar.txt";

    TEST_ASSERT_EQUAL(strlen(expected), pathExtension("/foo/bar.tar.gz", 15, "txt", 3, g_buffer, 255));
    TEST_ASSERT_EQUAL_STRING(expected, g_buffer);
}

void test_pathExtension_Should_ReturnZero_When_GivenPathLengthTooBig() {
    TEST_ASSERT_EQUAL(0, pathExtension("/foo/", 5, "txt", 3, g_buffer, 3));

    TEST_ASSERT_EQUAL(0, pathExtension("/foo", 4, "txt", 3, g_buffer, 4));
}

void test_reverseComplement_NotModifyInput_When_GivenZeroLength() {
    char input[] = "CGTACGT";

    reverseComplement(input, 0);

    TEST_ASSERT_EQUAL_STRING("CGTACGT", input);
}

void test_reverseComplement() {
    char input[] = "CGTACGT";

    reverseComplement(input, 7);

    TEST_ASSERT_EQUAL_STRING("ACGTACG", input);
}

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
    RUN_TEST(test_pathExtension_Should_ReturnSamePath_When_GivenZeroExtLength);
    RUN_TEST(test_pathExtension_Should_ReturnFilenameWithExt_When_GivenFilenameWithoutExt);
    RUN_TEST(test_pathExtension_Should_ReturSamePath_When_GivenPathWithoutFilename);
    RUN_TEST(test_pathExtension_Should_ReturnPathWithExt_When_GivenPathToFileWithoutExt);
    RUN_TEST(test_pathExtension_Should_ReturnPathWithModifiedExt_When_GivenPathToFileWithExt);
    RUN_TEST(test_pathExtension_Should_ReturnZero_When_GivenPathLengthTooBig);

    RUN_TEST(test_reverseComplement_NotModifyInput_When_GivenZeroLength);
    RUN_TEST(test_reverseComplement);
    RUN_TEST(test_reverseString_will_NotModifyInput_when_GivenZeroLength);
    RUN_TEST(test_reverseString_will_ModifyInputWithItsReverse);
    return UNITY_END();
}