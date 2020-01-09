#include "unity.h"

#include "queue.h"

static Queue *g_queue;

void setUp() {
    g_queue = NULL;
}

void tearDown() {
    queueDelete(g_queue);
}

void test_queueCreate_Should_ReturnValidPointer() {
    g_queue = queueCreate(10, 1);

    TEST_ASSERT_NOT_NULL(g_queue);
    TEST_ASSERT_TRUE(queueEmpty(g_queue));
    TEST_ASSERT_FALSE(queueFull(g_queue));
}

void test_queuePush_Should_MakeQueueNonEmpty() {
    g_queue = queueCreate(10, 1);

    TEST_ASSERT_NOT_NULL(g_queue);

    char value = 'A';
    TEST_ASSERT_TRUE(queuePush(g_queue, &value));
    TEST_ASSERT_FALSE(queueEmpty(g_queue));
}

void test_queuePush_Should_MakeQueueFullAndEmpty_When_GivenOneSizeQueue() {
    g_queue = queueCreate(1, 1);

    TEST_ASSERT_NOT_NULL(g_queue);

    char value = 'A';
    TEST_ASSERT_TRUE(queuePush(g_queue, &value));
    TEST_ASSERT_FALSE(queueEmpty(g_queue));
    TEST_ASSERT_TRUE(queueFull(g_queue));
}

void test_queuePop_Should_MakeQueueEmpty_When_GivenQueueWithOneElement() {
    g_queue = queueCreate(10, 1);

    TEST_ASSERT_NOT_NULL(g_queue);

    char value = 'A';
    TEST_ASSERT_TRUE(queuePush(g_queue, &value));

    char result = 0;
    TEST_ASSERT_TRUE(queuePop(g_queue, &result));
    TEST_ASSERT_TRUE(queueEmpty(g_queue));
}

void test_queueClear_Should_MakeQueueEmpty() {
    g_queue = queueCreate(10, 1);
    TEST_ASSERT_NOT_NULL(g_queue);

    char v1 = 'A';
    TEST_ASSERT_TRUE(queuePush(g_queue, &v1));
    char v2 = 'B';
    TEST_ASSERT_TRUE(queuePush(g_queue, &v2));

    queueClear(g_queue);
    TEST_ASSERT_TRUE(queueEmpty(g_queue));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_queueCreate_Should_ReturnValidPointer);
    RUN_TEST(test_queuePush_Should_MakeQueueNonEmpty);
    RUN_TEST(test_queuePush_Should_MakeQueueFullAndEmpty_When_GivenOneSizeQueue);
    RUN_TEST(test_queuePop_Should_MakeQueueEmpty_When_GivenQueueWithOneElement);
    RUN_TEST(test_queueClear_Should_MakeQueueEmpty);
    return UNITY_END();
}