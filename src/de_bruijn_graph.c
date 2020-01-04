#include "de_bruijn_graph.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include "bloom_filter.h"
#include "getline.h"
#include "log.h"
#include "string_utils.h"
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

    char *kmer = malloc(k);

    if (!kmer) {
        return false;
    }

    bool result = false;

    ssize_t lineLength;
    while ((lineLength = getline(&line, &length, fp)) > 0) {
        // Skips headers
        if (*line == '>') {
            continue;
        }

        if (k > lineLength) {
            goto EXIT;
        }

        // Inserts each kmer into the filter
        for (int64_t i = 0;i < lineLength - k + 1;i++) {
            // Each kmer starts at position i and has
            // a length of k. 
            // The line must be greater than k

            memcpy(kmer, line + i, k);
            if (!insertKmer(bf, kmer, k)) {
                log_error("Unable to insert kmer %.*s", k, kmer);
                goto EXIT;
            }
        }
    }

    if (!feof(fp) && lineLength < 0) {
        perror("something bad happened");
    }
    else {
        result = true;
    }

EXIT:
    free(kmer);
    free(line);
    return result;
}

bool insertKmer(struct BloomFilter *bf, char *kmer, int k) {
    assert(bf);
    assert(kmer);

    if (k <= 0) {
        return false;
    }

    if (!canonicalForm(kmer, k)) {
        return false;
    }

    return bfAdd(bf, kmer, k);
}

BloomFilter *loadDBG(gzFile fp) {
    assert(fp);

    int r;
    long size = 0;

    if ((r = gzread(fp, &size, 4)) != 4) {
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

    int8_t *bytes = malloc(sizeof(*bytes) * size);

    if (!bytes) {
        log_error("Unable to allocate a buffer of size %I64d", size);
        return NULL;
    }

    if ((r = gzread(fp, bytes, size)) != size) {
        if (r != -1) {
            log_error("Unable to read the content of the Bloom filter : %s", gzFileError(fp));
        }
        else {
            log_error("content error");
        }
        free(bytes);
        return NULL;
    }

    BloomFilter *bf = bfCreate(size, nbHashs);

    if (!bf) {
        log_error("Unable to create a new Bloom filter");
        free(bytes);
        return NULL;
    }

    bfFill(bf, bytes);

    free(bytes);
    return bf;
}

bool saveDBG(BloomFilter *bf, gzFile fp) {
    assert(bf);
    assert(fp);

    // @TODO check endianness

    long filterSize = bfSize(bf);
    if (gzwrite(fp, &filterSize, 4) <= 0) {
        log_error("Unable to write filter size : %s", gzFileError(fp));
        return false;
    }

    long filterHashs = bfNbHashs(bf);
    if (gzwrite(fp, &filterHashs, 1) <= 0) {
        log_error("Unable to write filter nbHashs : %s", gzFileError(fp));
        return false;
    }

    if (gzwrite(fp, bf->data, filterSize) < bf->size) {
        log_error("Unable to write filter content : %s", gzFileError(fp));
        return false;
    }

    return true;
}