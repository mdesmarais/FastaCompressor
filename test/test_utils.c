#include "utils.h"

#include "unity.h"

#include "bloom_filter.h"

static BloomFilter *g_bf;

void setUp() {
    g_bf = NULL;
}
void tearDown() {
    bfDelete(g_bf);
}

void test_canonicalForm_Should_ReturnNull_When_GivenZeroLengthString() {
    TEST_ASSERT_NULL(canonicalForm("ATCG", 0));
}

void test_canonicalForm_Should_ReturnPointerToKmerOrRC() {
    char kmer[] = "CGTACGT";

    char *result = canonicalForm(kmer, 7);
    TEST_ASSERT_NOT_NULL(result);

    TEST_ASSERT_EQUAL(kmer, result);
    TEST_ASSERT_EQUAL_STRING("ACGTACG", result);
}

void test_findNeighbors_Should_ReturnNegativeValue_When_GivenLengthLessThanTwo() {
    g_bf = bfCreate(100, 7);
    char neighbors[4];
    TEST_ASSERT_LESS_THAN(0, findNeighbors(g_bf, "A", 1, neighbors));
}

void test_findNeighbors_Should_ReturnZero_When_GivenEmptyFilter() {
    g_bf = bfCreate(100, 7);
    char neighbors[5] = { '\0' };

    TEST_ASSERT_EQUAL(0, findNeighbors(g_bf, "ATCG", 4, neighbors));

    TEST_ASSERT_EQUAL_STRING("", neighbors);
}

void test_findNeighbors_Should_ReturnOne_When_GivenFilterWithOneElement() {
    g_bf = bfCreate(100, 7);
    char neighbors[5] = { '\0' };

    bfAdd(g_bf, "CCGA", 4);

    TEST_ASSERT_EQUAL(1, findNeighbors(g_bf, "ATCG", 4, neighbors));

    TEST_ASSERT_EQUAL_STRING("G", neighbors);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_canonicalForm_Should_ReturnNull_When_GivenZeroLengthString);
    RUN_TEST(test_canonicalForm_Should_ReturnPointerToKmerOrRC);
    RUN_TEST(test_findNeighbors_Should_ReturnNegativeValue_When_GivenLengthLessThanTwo);
    RUN_TEST(test_findNeighbors_Should_ReturnZero_When_GivenEmptyFilter);
    RUN_TEST(test_findNeighbors_Should_ReturnOne_When_GivenFilterWithOneElement);
    return UNITY_END();
}