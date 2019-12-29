#include "vector.h"

#include "unity.h"

static Vector *g_vec;

void setUp() {
    g_vec = NULL;
}
void tearDown() {
    vectorDelete(g_vec);
}

void test_vectorCreate() {
    g_vec = vectorCreate(0, sizeof(int));

    TEST_ASSERT_NOT_NULL(g_vec);
    TEST_ASSERT_EQUAL(0, vectorCapacity(g_vec));
    TEST_ASSERT_EQUAL(0, vectorSize(g_vec));
}

void test_vectorPush_Should_ReturnTrue_And_UpdateVectorSize() {
    g_vec = vectorCreate(2, sizeof(int));

    int v1 = 2;
    TEST_ASSERT_TRUE(vectorPush(g_vec, &v1));
    TEST_ASSERT_EQUAL(1, vectorSize(g_vec));

    int v2 = 1;
    TEST_ASSERT_TRUE(vectorPush(g_vec, &v2));
    TEST_ASSERT_EQUAL(2, vectorSize(g_vec));

    TEST_ASSERT_EQUAL(2, vectorCapacity(g_vec));
}

void test_vectorPush_Should_ResizeVector_When_CapacityLowerThanSize() {
    g_vec = vectorCreate(0, sizeof(int));

    int v = 5;
    TEST_ASSERT_TRUE(vectorPush(g_vec, &v));

    TEST_ASSERT_EQUAL(1, vectorSize(g_vec));
    TEST_ASSERT_GREATER_OR_EQUAL(1, vectorCapacity(g_vec));
}

void test_vectorAt_Should_ReturnNull_When_GivenIndexOutOfSize() {
    g_vec = vectorCreate(2, sizeof(int));

    TEST_ASSERT_NULL(vectorAt(g_vec, 2));
}

void test_vectorAt_Should_ReturnPointerToValue() {
    g_vec = vectorCreate(4, sizeof(int));

    int v1 = 1;
    TEST_ASSERT_TRUE(vectorPush(g_vec, &v1));

    int v2 = 2;
    TEST_ASSERT_TRUE(vectorPush(g_vec, &v2));

    int *result1 = (int*) vectorAt(g_vec, 0);
    TEST_ASSERT_NOT_NULL(result1);
    TEST_ASSERT_EQUAL(v1, *result1);

    int *result2 = (int*) vectorAt(g_vec, 1);
    TEST_ASSERT_NOT_NULL(result2);
    TEST_ASSERT_EQUAL(v2, *result2);
}

void test_vectorAt_Should_ReturnPointerToACopyOfTheValue() {
    g_vec = vectorCreate(4, sizeof(int));

    int v = 5;
    TEST_ASSERT_TRUE(vectorPush(g_vec, &v));

    int *p = (int*) vectorAt(g_vec, 0);
    TEST_ASSERT_NOT_NULL(p);
    *p = 1;

    TEST_ASSERT_EQUAL(5, v);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_vectorCreate);
    RUN_TEST(test_vectorPush_Should_ReturnTrue_And_UpdateVectorSize);
    RUN_TEST(test_vectorPush_Should_ResizeVector_When_CapacityLowerThanSize);
    RUN_TEST(test_vectorAt_Should_ReturnNull_When_GivenIndexOutOfSize);
    RUN_TEST(test_vectorAt_Should_ReturnPointerToValue);
    RUN_TEST(test_vectorAt_Should_ReturnPointerToACopyOfTheValue);
    return UNITY_END();
}