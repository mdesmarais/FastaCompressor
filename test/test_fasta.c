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

/*void test_computeBranchings_Should_ReturnFalse_When_GivenNegativeK() {
    g_bf = bfCreate(10000, 7);
    g_vec = vectorCreate(10, 1);

    TEST_ASSERT_TRUE(bfAdd(g_bf, "CTGACG", 6));
    TEST_ASSERT_TRUE(bfAdd(g_bf, "TGACGT", 6));
    TEST_ASSERT_TRUE(bfAdd(g_bf, "GACGTG", 6));
    TEST_ASSERT_TRUE(bfAdd(g_bf, "ACGTGG", 6));
    TEST_ASSERT_TRUE(bfAdd(g_bf, "CGTGGA", 6));

    TEST_ASSERT_TRUE(computeBranchings(g_bf, g_vec, "CTGACGTGGA", 10, 6));

    TEST_ASSERT_EQUAL(1, vectorSize(g_vec));
}*/

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

int main() {
    UNITY_BEGIN();
    //RUN_TEST(test_computeBranchings_Should_ReturnFalse_When_GivenNegativeK);
    RUN_TEST(test_decompressRead_Should_ReturnTrue_When_GivenValidCompressedRead);
    return UNITY_END();
}