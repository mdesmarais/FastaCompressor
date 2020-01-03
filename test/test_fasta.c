#include "unity.h"

#include "bloom_filter.h"
#include "de_bruijn_graph.h"
#include "fasta.h"
#include "vector.h"

#include <string.h>

static BloomFilter *g_bf;
static Vector *g_vec;

void setUp() {
    g_bf = NULL;
    g_vec = NULL;
}

void tearDown() {
    bfDelete(g_bf);
    vectorDelete(g_vec);
}

void test_computeBranchings() {
    g_bf = bfCreate(10000, 7);
    g_vec = vectorCreate(10, 1);

    char k1[] = "CTGACG";
    char k2[] = "TGACGT";
    char k3[] = "GACGTG";
    char k4[] = "ACGTGG";
    char k5[] = "CGTGGA";
    char k6[] = "ACGTGT";

    TEST_ASSERT_TRUE(insertKmer(g_bf, k1, 6));
    TEST_ASSERT_TRUE(insertKmer(g_bf, k2, 6));
    TEST_ASSERT_TRUE(insertKmer(g_bf, k3, 6));
    TEST_ASSERT_TRUE(insertKmer(g_bf, k4, 6));
    TEST_ASSERT_TRUE(insertKmer(g_bf, k5, 6));
    TEST_ASSERT_TRUE(insertKmer(g_bf, k6, 6));

    char seq[] = "CTGACGTGGA";
    TEST_ASSERT_TRUE(computeBranchings(g_bf, g_vec, seq, 10, 6));

    TEST_ASSERT_EQUAL(1, vectorSize(g_vec));
}

void test_decompressRead_Should_ReturnTrue_When_GivenValidCompressedRead() {
    g_bf = bfCreate(10000, 7);
    g_vec = vectorCreate(10, 1);

    TEST_ASSERT_NOT_NULL(g_bf);
    TEST_ASSERT_NOT_NULL(g_vec);

    char seq1[] = "ATTTCGGGAAAAAATCGAGCCCTAATT";
    char seq2[] = "CGGGAAAAAATCGAGCCCTAATTATTT";
    char kmer[8];

    for (int i = 0;i < strlen(seq1);i++) {
        memcpy(kmer, seq1 + i, 8);
        TEST_ASSERT_TRUE(insertKmer(g_bf, kmer, 8));

        memcpy(kmer, seq2 + i, 8);
        TEST_ASSERT_TRUE(insertKmer(g_bf, kmer, 8));
    }

    char branchings[] = { 'T', 'C' };
    TEST_ASSERT_TRUE(vectorPush(g_vec, &branchings[0]));
    TEST_ASSERT_TRUE(vectorPush(g_vec, &branchings[1]));

    char result[28] = { '\0' };

    TEST_ASSERT_TRUE(decompressRead(g_bf, g_vec, result, 27, "ATTTCGGG", 8));
    TEST_ASSERT_EQUAL_STRING(seq1, result);
}

void test_extractBranchings_Should_ReturnZero_When_GivenLineWithoutBranchings() {
    g_vec = vectorCreate(10, 1);
    TEST_ASSERT_NOT_NULL(g_vec);

    char l1[] = "helloworl";
    TEST_ASSERT_EQUAL(0, extractBranchings(g_vec, l1));

    char l2[] = "hello ";
    TEST_ASSERT_EQUAL(0, extractBranchings(g_vec, l2));
}

void test_extractBranchings_Should_NotModifyVector_When_GivenLineWithoutBranchings() {
    g_vec = vectorCreate(10, 1);
    TEST_ASSERT_NOT_NULL(g_vec);

    size_t initialSize = vectorSize(g_vec);

    char l1[] = "helloworl";
    TEST_ASSERT_GREATER_OR_EQUAL(0, extractBranchings(g_vec, l1));
    TEST_ASSERT_EQUAL(initialSize, vectorSize(g_vec));

    char l2[] = "hello ";
    TEST_ASSERT_GREATER_OR_EQUAL(0, extractBranchings(g_vec, l2));
    TEST_ASSERT_EQUAL(initialSize, vectorSize(g_vec));
}

void test_extractBranchings_Should_ReturnTwo_And_UpdateVector_When_GivenLineWithTwoBranchings() {
    g_vec = vectorCreate(10, 1);
    TEST_ASSERT_NOT_NULL(g_vec);

    char l1[] = "hello ab";
    TEST_ASSERT_EQUAL(2, extractBranchings(g_vec, l1));
    TEST_ASSERT_EQUAL(2, vectorSize(g_vec));

    char *v1 = vectorAt(g_vec, 0);
    TEST_ASSERT_NOT_NULL(v1);
    TEST_ASSERT_EQUAL('a', *v1);

    char *v2 = vectorAt(g_vec, 1);
    TEST_ASSERT_NOT_NULL(v2);
    TEST_ASSERT_EQUAL('b', *v2);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_computeBranchings);

    RUN_TEST(test_decompressRead_Should_ReturnTrue_When_GivenValidCompressedRead);

    RUN_TEST(test_extractBranchings_Should_ReturnZero_When_GivenLineWithoutBranchings);
    RUN_TEST(test_extractBranchings_Should_NotModifyVector_When_GivenLineWithoutBranchings);
    RUN_TEST(test_extractBranchings_Should_ReturnTwo_And_UpdateVector_When_GivenLineWithTwoBranchings);
    return UNITY_END();
}