#include "bloom_filter.h"

#include "log.h"
#include "murmur3.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \brief Gets the bit position associated to the given hash
 * 
 * hash is a pointer to a uint32_t value. It could have several values (more than 32 bits).
 * The parameter len represents this number.
 * 
 * @param hash pointer to the first hash value
 * @param len number of values
 * @param n size of the filter (in bytes)
 * @return index of a bit
 */
static long getPositionFromHash(uint32_t *hash, size_t len, long n) {
    assert(hash);

    long pos = 0;

    for (size_t i = 0;i < len;i++) {
        pos = (pos + hash[i]) % n;
    }

    return pos;
}

BloomFilter *bfCreate(long n, int8_t k) {
    if (k <= 0) {
        return NULL;
    }

    char *data = calloc(n, sizeof(*data));

    if (!data) {
        log_error("Filter internal array allocation error");
        return NULL;
    }

    BloomFilter *bf = malloc(sizeof(*bf));

    if (!bf) {
        free(data);
        log_error("Filter allocation error");
        return NULL;
    }

    bf->data = data;
    bf->nbhashs = k;
    bf->size = n;

    return bf;
}

void bfDelete(BloomFilter *bf) {
    if (bf) {
        free(bf->data);
        free(bf);
    }
}

void bfFill(BloomFilter *bf, int8_t *bytes) {
    assert(bf);
    assert(bytes);

    memcpy(bf->data, bytes, bfSize(bf));
}

char bfGetBit(BloomFilter *bf, long long i, int *pError) {
    assert(bf);

    if (i < 0 || i >= bfSize(bf) * 8) {
        if (pError) {
            *pError = 1;
        }

        return 0;
    }

    size_t offset = i / 8;
    long position = i % 8;

    if (pError) {
        *pError = 0;
    }

    char *byte = bf->data + offset;
    return (*byte >> position) & 0x1;
}

bool bfSetBit(BloomFilter *bf, long long i) {
    assert(bf);

    if (i >= bfSize(bf) * 8) {
        return false;
    }

    size_t offset = i / 8;
    long position = i % 8;

    char *byte = bf->data + offset;
    long mask = 1 << position;

    *byte = *byte | mask;

    return true;
}

bool bfAdd(BloomFilter *bf, void *value, int valSize) {
    assert(bf);
    assert(value);

    if (valSize <= 0) {
        return false;
    }

    uint32_t hash[4];

    for (int i = 0;i < bfNbHashs(bf);i++) {
        // The hash is 128 bits long, we store it in an array
        // of 4 unsigned 32 bits integers
        MurmurHash3_x64_128(value, valSize, i, hash);

        // Retreives bit position for this hash
        long pos = getPositionFromHash(hash, 4, bfSize(bf));

        if (!bfSetBit(bf, pos)) {
            return false;
        }
    }

    return true;
}

bool bfContains(BloomFilter *bf, void *value, int valSize) {
    assert(bf);
    assert(value);

    if (valSize <= 0) {
        return false;
    }

    uint32_t hash[4];
    int error;

    for (int i = 0;i < bfNbHashs(bf);i++) {
        MurmurHash3_x64_128(value, valSize, i, hash);

        long pos = getPositionFromHash(hash, 4, bfSize(bf));

        char b = bfGetBit(bf, pos, &error);

        if (error != 0) {
            log_error("bfContains : Error while trying to get a bit value %d", error);
            return false;
        }

        if (!b) {
            return false;
        }
    }

    return true;
}
