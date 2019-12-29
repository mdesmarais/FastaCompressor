#include "fasta.h"

#include "bloom_filter.h"
#include "utils.h"
#include "vector.h"

#include <assert.h>
#include <string.h>

bool computeBranchings(BloomFilter *bf, Vector *v, char *seq, size_t len, int k) {
    assert(bf);
    assert(v);
    assert(seq);

    if (k <= 0) {
        return false;
    }

    char neighbors[4];

    for (size_t i = 0;i < len - k + 1;i++) {
        int nbNeighbors = findNeighbors(bf, seq + i, k, neighbors);

        if (nbNeighbors < 0) {
            return false;
        }

        if (nbNeighbors == 0) {
            break;
        }
        else if (nbNeighbors > 1) {
            vectorPush(v, seq + i);
        }
    }

    return true;
}