#include "unity.h"

#include "bloom_filter.h"

BloomFilter *g_bf = NULL;

void setUp() {
    g_bf = NULL;
}

void tearDown() {
    bfDelete(g_bf);
}

void test_bfCreate_Should_ReturnNull_When_GivenNegativeK() {
    TEST_ASSERT_NULL(bfCreate(5, 0));
    TEST_ASSERT_NULL(bfCreate(5, -1));
}

void test_bfCreate_Should_ReturnValidFilter() {
    g_bf = bfCreate(8, 3);

    TEST_ASSERT_EQUAL(8, bfSize(g_bf));
    TEST_ASSERT_EQUAL(3, bfNbHashs(g_bf));

    for (size_t i = 0;i < 8;i++) {
        TEST_ASSERT_EQUAL(0, bfGetBit(g_bf, i, NULL));
    }
}

void test_bfGetBit_Should_ReturnError_When_GivenIndexOutOfBounds() {
    g_bf = bfCreate(8, 3);

    int error = 0;
    bfGetBit(g_bf, 9, &error);

    TEST_ASSERT_NOT_EQUAL(0, error);
}

void test_bfSetBit_Should_ReturnFalse_When_GivenIndexOutOfBounds() {
    g_bf = bfCreate(8, 3);

    TEST_ASSERT_EQUAL(false, bfSetBit(g_bf, 9));
}

void test_bfGetBit_bfSetBit() {
    g_bf = bfCreate(9, 3);

    TEST_ASSERT_EQUAL(true, bfSetBit(g_bf, 1));
    TEST_ASSERT_EQUAL(true, bfSetBit(g_bf, 8));

    int error = 0;

    for (size_t i = 0;i < 9;i++) {
        if (i == 1 || i == 8) {
            TEST_ASSERT_EQUAL(1, bfGetBit(g_bf, i, &error));
        }
        else {
            TEST_ASSERT_EQUAL(0, bfGetBit(g_bf, 0, &error));
        }

        TEST_ASSERT_EQUAL(0, error);
    }
}

void test_bfAdd_Should_UpdateAtLeastOneBitMaxNbHashs() {
    g_bf = bfCreate(8, 2);

    bfAdd(g_bf, "foo", 3);

    int updatedBits = 0;

    for (size_t i = 0;i < 8;i++) {
        if (bfGetBit(g_bf, i, NULL)) {
            updatedBits++;
        }
    }

    TEST_ASSERT_GREATER_THAN(0, updatedBits);
    TEST_ASSERT_LESS_OR_EQUAL(bfNbHashs(g_bf), updatedBits);
}

void test_bfContains_Should_ReturnFalse_When_GivenEmptyFilter() {
    g_bf = bfCreate(8, 2);

    TEST_ASSERT_EQUAL(false, bfContains(g_bf, "foo", 3));
}

void test_bfContains_Should_ReturnFalse_When_GivenUnknownHash() {
    g_bf = bfCreate(8, 2);

    bfAdd(g_bf, "bar", 3);

    TEST_ASSERT_EQUAL(false, bfContains(g_bf, "foo", 3));
}

void test_bfContains_Should_ReturnTrue_When_GivenExistingHash() {
    g_bf = bfCreate(8, 2);

    bfAdd(g_bf, "bar", 3);

    TEST_ASSERT_EQUAL(true, bfContains(g_bf, "bar", 3));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_bfCreate_Should_ReturnNull_When_GivenNegativeK);
    RUN_TEST(test_bfCreate_Should_ReturnValidFilter);
    RUN_TEST(test_bfGetBit_Should_ReturnError_When_GivenIndexOutOfBounds);
    RUN_TEST(test_bfSetBit_Should_ReturnFalse_When_GivenIndexOutOfBounds);
    RUN_TEST(test_bfGetBit_bfSetBit);
    RUN_TEST(test_bfAdd_Should_UpdateAtLeastOneBitMaxNbHashs);
    RUN_TEST(test_bfContains_Should_ReturnFalse_When_GivenEmptyFilter);
    RUN_TEST(test_bfContains_Should_ReturnFalse_When_GivenUnknownHash);
    RUN_TEST(test_bfContains_Should_ReturnTrue_When_GivenExistingHash);
    return UNITY_END();
}