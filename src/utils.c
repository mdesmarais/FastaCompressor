#include "utils.h"

#include "bloom_filter.h"
#include "string_utils.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *canonicalForm(char *kmer, size_t len) {
    assert(kmer);

    if (len == 0) {
        return NULL;
    }

    char *rc = malloc(sizeof(*rc) * len);
    if (!rc) {
        return NULL;
    }

    memcpy(rc, kmer, len);
    reverseComplement(rc, len);

    if (strncmp(kmer, rc, len) > 0) {
        // The reverse-complement is before the kmer
        // in the lexicographic order
        memcpy(kmer, rc, len);
    }

    free(rc);

    return kmer;
}

const char *gzFileError(gzFile fp) {
    assert(fp);

    int errnum;
    const char *str = gzerror(fp, &errnum);

    if (errnum == Z_ERRNO) {
        return strerror(errno);
    }

    return str;
}

int findNeighbors(BloomFilter *bf, const char *kmer, size_t len, char *neighbors) {
    assert(bf);
    assert(kmer);
    assert(neighbors);

    if (len < 2) {
        return -1;
    }

    // Will contain the last 3 letters of the kmer + an additional letter
    char *nextKmer = malloc(sizeof(*nextKmer) * len);

    if (!nextKmer) {
        return -1;
    }

    int nbNeighbors = 0;
    char letters[] = { 'A', 'T', 'C', 'G' };

    for (int i = 0;i < 4;i++) {
        // Copies the last 3 letters of the kmer
        memcpy(nextKmer, kmer + 1, len - 1);

        // Adds the final letter
        nextKmer[len - 1] = letters[i];

        // Computes the canonical form (nextKmer will be modified)
        canonicalForm(nextKmer, len);

        if (bfContains(bf, nextKmer, len)) {
            // Adds the letter into the container if the current next kmer
            // is in the filter
            neighbors[nbNeighbors++] = letters[i];
        }
    }

    free(nextKmer);
    return nbNeighbors;
}