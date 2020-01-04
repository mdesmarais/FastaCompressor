#include "bloom_filter.h"

#include "log.h"
#include "murmur3.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *getByte(BloomFilter *bf, int64_t i) {
    assert(bf);
    assert(i < bf->size);

    return &bf->data[i];
}

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
        return NULL;
    }

    BloomFilter *bf = malloc(sizeof(*bf));

    if (!bf) {
        free(data);

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

void bfFill(BloomFilter *bf, int8_t *bits) {
    assert(bf);
    assert(bits);

    memcpy(bf->data, bits, bfSize(bf));
}

char bfGetBit(BloomFilter *bf, long long i, int *pError) {
    assert(bf);

    if (i >= bfSize(bf)) {
        if (pError) {
            *pError = 1;
        }

        return 0;
    }

    long offset = i / 8;
    long position = i % 8;

    if (pError) {
        *pError = 0;
    }

    char *byte = getByte(bf, offset);
    return (*byte >> position) & 0x1;
}

bool bfSetBit(BloomFilter *bf, long long i) {
    assert(bf);

    if (i >= bfSize(bf)) {
        return false;
    }

    long offset = i / 8;
    long position = i % 8;

    char *byte = getByte(bf, offset);
    long mask = 1 << position;

    *byte = *byte | mask;

    return true;
}

bool bfAdd(BloomFilter *bf, void *value, int len) {
    assert(bf);
    assert(value);

    if (len <= 0) {
        return false;
    }

    long size = bfSize(bf);
    int nbHashs = bfNbHashs(bf);
    uint32_t hash[4];

    for (int i = 0;i < nbHashs;i++) {
        MurmurHash3_x64_128(value, len, i, hash);

        long pos = getPositionFromHash(hash, 4, size);

        if (!bfSetBit(bf, pos)) {
            return false;
        }
    }

    return true;
}

bool bfContains(BloomFilter *bf, void *value, int len) {
    assert(bf);
    assert(value);

    if (len <= 0) {
        return false;
    }

    long size = bfSize(bf);
    int nbHashs = bfNbHashs(bf);
    uint32_t hash[4];
    int error;

    for (int i = 0;i < nbHashs;i++) {
        MurmurHash3_x64_128(value, len, i, hash);

        long pos = getPositionFromHash(hash, 4, size);

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
