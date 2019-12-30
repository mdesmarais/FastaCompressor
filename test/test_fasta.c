#include "unity.h"

#include "bloom_filter.h"
#include "fasta.h"
#include "vector.h"

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

int main() {
    UNITY_BEGIN();
    //RUN_TEST(test_computeBranchings_Should_ReturnFalse_When_GivenNegativeK);
    return UNITY_END();
}