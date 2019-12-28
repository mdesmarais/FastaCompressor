#include "file_buffer.h"

#include "unity.h"

#include <stdbool.h>
#include <stdio.h>

FileBuffer *g_fb;

void setUp() {
    g_fb = NULL;
}

void tearDown() {
    fbDelete(g_fb);
    remove("tmp.data");
}

void test_fbCreate() {
    g_fb = fbCreate(stdin, 10, 5);

    TEST_ASSERT_NOT_NULL(g_fb);
    TEST_ASSERT_NOT_NULL(g_fb->inBuf);
    TEST_ASSERT_NOT_NULL(g_fb->outBuf);

    TEST_ASSERT_EQUAL(0, g_fb->inBufSize);
    TEST_ASSERT_EQUAL(0, g_fb->outBufSize);

    TEST_ASSERT_EQUAL(10, g_fb->inBufCapacity);
    TEST_ASSERT_EQUAL(5, g_fb->outBufCapacity);
}

void test_fbWrite() {
    FILE *fp = fopen("tmp.data", "w+");

    if (!fp) {
        perror("Unable to open a temporary file");
        TEST_FAIL();
    }

    g_fb = fbCreate(fp, 10, 5);

    char input[] = "Hello World";

    TEST_ASSERT_EQUAL(true, fbWrite(g_fb, input, 11));
    fbFlush(g_fb);
    TEST_ASSERT_EQUAL(0, g_fb->outBufSize);

    char result[12];
    fseek(fp, 0, 0);
    size_t resultSize = fread(result, sizeof(*result), 11, fp);

    TEST_ASSERT_EQUAL(11, resultSize);
    TEST_ASSERT_EQUAL_STRING(input, result);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_fbCreate);
    RUN_TEST(test_fbWrite);
    return UNITY_END();
}