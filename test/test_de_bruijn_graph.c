#include "unity.h"

#include "bloom_filter.h"
#include "de_bruijn_graph.h"
#include "utils.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

static BloomFilter *g_bf;
static gzFile g_fp;

bool openTestFile(const char *mode) {
    if ((g_fp = gzopen("test_dbg.dat", mode)) == NULL) {
        if (errno == 0) {
            fprintf(stderr, "Unable to create test file (gzip error)\n");
        }
        else {
            perror("Unable to open test file");
        }

        return false;
    }

    return true;
}

void setUp() {
    g_bf = NULL;
    
    TEST_ASSERT_TRUE(openTestFile("wb"));
    gzclose(g_fp);
    g_fp = NULL;
}

void tearDown() {
    bfDelete(g_bf);
    gzclose(g_fp);
    //remove("test_dbg.dat");
}

void test_loadDBG_Should_ReturnNull_When_GivenEmptyFile() {
    TEST_ASSERT_TRUE(openTestFile("rb"));
    TEST_ASSERT_NULL(loadDBG(g_fp));
}

void test_loadDBG_Should_ReturnNull_When_MissingData() {
    g_bf = bfCreate(100, 3);
    TEST_ASSERT_NOT_NULL(g_bf);

    TEST_ASSERT_TRUE(openTestFile("wb"));
    TEST_ASSERT_TRUE(saveDBG(g_bf, g_fp));
    bfDelete(g_bf);
    gzclose(g_fp);

    TEST_ASSERT_TRUE(openTestFile("rb"));

    // The last byte of the filter will be missing
    gzseek(g_fp, 1, SEEK_SET);

    g_bf = loadDBG(g_fp);
    TEST_ASSERT_NULL(g_bf);
}

void test_loadDBG_saveDBG() {
    g_bf = bfCreate(10000, 3);
    TEST_ASSERT_NOT_NULL(g_bf);

    // Changes one bit
    TEST_ASSERT_TRUE(bfSetBit(g_bf, 500));

    TEST_ASSERT_TRUE(openTestFile("wb"));
    TEST_ASSERT_TRUE(saveDBG(g_bf, g_fp));
    bfDelete(g_bf);
    gzclose(g_fp);

    TEST_ASSERT_TRUE(openTestFile("rb"));
    g_bf = loadDBG(g_fp);

    TEST_ASSERT_NOT_NULL(g_bf);

    TEST_ASSERT_EQUAL(10000, bfSize(g_bf));
    TEST_ASSERT_EQUAL(3, bfNbHashs(g_bf));

    int error;
    for (int64_t i = 0;i < 10000;i++) {
        char b = bfGetBit(g_bf, i, &error);
        TEST_ASSERT_EQUAL(0, error);

        if (i == 500) {
            TEST_ASSERT_EQUAL(1, b);
        }
        else {
            TEST_ASSERT_EQUAL(0, b);
        }
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_loadDBG_Should_ReturnNull_When_GivenEmptyFile);
    RUN_TEST(test_loadDBG_Should_ReturnNull_When_MissingData);
    RUN_TEST(test_loadDBG_saveDBG);
    return UNITY_END();
}