#include "de_bruijn_graph.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include "bloom_filter.h"
#include "getline.h"
#include "log.h"
#include "utils.h"

bool createDBG(BloomFilter *bf, FILE *fp, int k) {
    assert(bf);
    assert(fp);

    // A kmer must have a positive length
    if (k <= 0) {
        return false;
    }

    char *line = NULL;
    size_t length = 0;

    ssize_t result;
    while ((result = getline(&line, &length, fp)) > 0) {
        // Skips headers
        if (*line == '>') {
            continue;
        }

        // Inserts each kmer into the filter
        for (int64_t i = 0;i < length - k + 1;i++) {
            // Each kmer starts at position i and has
            // a length of k. 
            // The line must be greater than k
            if (k > result || !bfAdd(bf, line + i, k)) {
                free(line);
                return false;
            }
        }
    }

    free(line);

    if (!feof(fp) && result < 0) {
        perror("something bad happened");
        return false;
    }

    return true;
}

BloomFilter *loadDBG(gzFile fp) {
    assert(fp);

    z_size_t r;
    int64_t size = 0;

    if ((r = gzread(fp, &size, sizeof(size))) != sizeof(size)) {
        if (r != -1) {
            log_error("Unable to read the size of the graph : %s", gzFileError(fp));
        }
        else {
            log_error("size error");
        }
        return NULL;
    }

    int8_t nbHashs = 0;
    if ((r = gzread(fp, &nbHashs, 1)) != 1) {
        if (r != -1) {
            log_error("Unable to read the number of hashs : %s", gzFileError(fp));
        }
        else {
            log_error("nbhashs error");
        }
        return NULL;
    }

    int8_t *bits = malloc(sizeof(*bits) * size);

    if (!bits) {
        log_error("Unable to allocate a buffer of size %I64d", size);
        return NULL;
    }

    if ((r = gzread(fp, bits, size)) != size) {
        if (r != -1) {
            log_error("Unable to read the content of the Bloom filter : %s", gzFileError(fp));
        }
        else {
            log_error("content error");
        }
        free(bits);
        return NULL;
    }

    BloomFilter *bf = bfCreate(size, nbHashs);

    if (!bf) {
        log_error("Unable to create a new Bloom filter");
        free(bits);
        return NULL;
    }

    bfFill(bf, bits);

    free(bits);
    return bf;
}

bool saveDBG(BloomFilter *bf, gzFile fp) {
    assert(bf);
    assert(fp);

    // @TODO check endianness

    if (gzwrite(fp, &bf->size, sizeof(bf->size)) <= 0) {
        log_error("Unable to write filter size : %s", gzFileError(fp));
        return false;
    }

    if (gzwrite(fp, &bf->nbhashs, sizeof(bf->nbhashs)) <= 0) {
        log_error("Unable to write filter nbHashs : %s", gzFileError(fp));
        return false;
    }

    if (gzwrite(fp, bf->data, sizeof(*bf->data) * bf->size) < bf->size) {
        log_error("Unable to write filter content : %s", gzFileError(fp));
        return false;
    }

    return true;
}