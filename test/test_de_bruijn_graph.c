#include "unity.h"

#include "bloom_filter.h"
#include "de_bruijn_graph.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static BloomFilter *g_bf;
static FILE *g_fp;

void setUp() {
    g_bf = NULL;

    if ((g_fp = fopen("test_dbg.dat", "wb+")) == NULL) {
        perror("Unable to create a test file");
        TEST_FAIL();
    }
}

void tearDown() {
    bfDelete(g_bf);
    fclose(g_fp);
    remove("test_dbg.dat");
}

void test_saveDBG() {
    g_bf = bfCreate(10000, 3);

    bool result = saveDBG(g_bf, g_fp);

    TEST_ASSERT_TRUE(result);

    fseek(g_fp, 0, 0);
    
    int64_t size = 0;
    TEST_ASSERT_EQUAL(1, fread(&size, sizeof(size), 1, g_fp));
    TEST_ASSERT_EQUAL(size, bfSize(g_bf));

    int nbhashs = 0;
    TEST_ASSERT_EQUAL(1, fread(&nbhashs, 1, 1, g_fp));
    TEST_ASSERT_EQUAL(3, nbhashs);
}

void test_loadDBG_Should_ReturnNull_When_GivenEmptyFile() {
    TEST_ASSERT_NULL(loadDBG(g_fp));
}

void test_loadDBG_Should_ReturnNull_When_MissingData() {
    int64_t size = 10000;
    int8_t nbHashs = 3;

    TEST_ASSERT_EQUAL(1, fwrite(&size, sizeof(size), 1, g_fp));
    TEST_ASSERT_EQUAL(1, fwrite(&nbHashs, sizeof(nbHashs), 1, g_fp));

    int8_t *bits = malloc(size);
    TEST_ASSERT_NOT_NULL(bits);

    TEST_ASSERT_EQUAL(size - 1, fwrite(bits, 1, size - 1, g_fp));

    BloomFilter *bf = loadDBG(g_fp);

    free(bits);

    if (bf) {
        bfDelete(bf);
        TEST_FAIL_MESSAGE("loadDBG should have returned a null pointer");
    }
}

void test_loadDBG_Should_ReturnValidPointer_When_GivenValidFile() {
    g_bf = bfCreate(10000, 3);
    TEST_ASSERT_NOT_NULL(g_bf);

    // Changes one bit
    TEST_ASSERT_TRUE(bfSetBit(g_bf, 500));

    TEST_ASSERT_TRUE(saveDBG(g_bf, g_fp));
    bfDelete(g_bf);

    fseek(g_fp, 0, 0);
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
    RUN_TEST(test_saveDBG);
    RUN_TEST(test_loadDBG_Should_ReturnNull_When_GivenEmptyFile);
    RUN_TEST(test_loadDBG_Should_ReturnNull_When_MissingData);
    RUN_TEST(test_loadDBG_Should_ReturnValidPointer_When_GivenValidFile);
    return UNITY_END();
}