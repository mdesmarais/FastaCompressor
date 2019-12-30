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
        //log_info("Neighbors : %d", nbNeighbors);

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