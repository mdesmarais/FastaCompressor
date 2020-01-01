#include "fasta.h"

#include "bloom_filter.h"
#include "getline.h"
#include "log.h"
#include "utils.h"
#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

bool compressFile(BloomFilter *bf, FILE *in, FILE *out, int k) {
    assert(bf);
    assert(in);
    assert(out);

    if (k <= 0) {
        return false;
    }

    Vector *v = vectorCreate(100, 1);

    if (!v) {
        log_error("Unable to create a new vector");
        return false;
    }

    char *line = NULL;
    size_t size = 0;

    ssize_t result;
    while ((result = getline(&line, &size, in)) > 0) {
        if (*line == '>') {
            continue;
        }

        if (k > result) {
            return false;
        }

        vectorClear(v);
        if (!computeBranchings(bf, v, line, result, k)) {
            log_error("branchings computation error");
            break;
        }

        char c = ' ';
        fwrite(line, k, 1, out);
        fwrite(&c, 1, 1, out);
        fwrite(vectorRawValues(v), 1, v->size, out);

        c = '\n';
        fwrite(&c, 1, 1, out);
    }

    free(line);
    vectorDelete(v);

    return true;
}

bool decompressRead(BloomFilter *bf, Vector *branchings, char *read, int readLength, const char *firstKmer, int k) {
    assert(bf);
    assert(branchings);
    assert(read);
    assert(firstKmer);

    if (readLength <= 0 || k <= 0) {
        return false;
    }

    char *kmer = malloc(k);
    bool result = false;

    if (!kmer) {
        log_error("Allocation failed");
        return false;
    }

    memcpy(kmer, firstKmer, k);
    memcpy(read, kmer, k);

    char neighbors[4];
    int nextBranching = 0;

    for (int i = 0;i < readLength - k;i++) {
        int nbNeighbors = findNeighbors(bf, kmer, k, neighbors);
        int neighborIndex = -1;

        if (nbNeighbors > 1) {
            char *pBranching = vectorAt(branchings, nextBranching);

            if (!pBranching) {
                log_error("Unable to get branching at index %d", nextBranching);
                goto EXIT;
            }

            // Checks if the current branching corresponds to one of the kmer neighbors
            for (int j = 0;j < nbNeighbors && neighborIndex == -1;j++) {
                if (*pBranching == neighbors[j]) {
                    neighborIndex = 0;
                }
            }

            if (neighborIndex == -1) {
                log_error("Branching error");
                return false;
            }

            nextBranching++;
        }
        else if (nbNeighbors == 1) {
            neighborIndex = 0;
        }
        else if (i == readLength - k) {
            // It was the last kmer
            break;
        }
        else {
            // Error
            log_error("Kmer without neighbors at %d", i);
            goto EXIT;
        }

        memcpy(kmer, kmer + 1, k - 1);
        kmer[k - 1] = neighbors[neighborIndex];
        read[i + k] = neighbors[neighborIndex];
    }

    result = true;

EXIT:
    free(kmer);

    return result;
}

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