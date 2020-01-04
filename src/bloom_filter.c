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

static int64_t getPositionFromHash(uint32_t *hash, size_t len, int64_t n) {
    assert(hash);

    int64_t pos = 0;

    for (size_t i = 0;i < len;i++) {
        pos = (pos + hash[i]) % n;
    }

    return pos;
}

BloomFilter *bfCreate(int64_t n, int8_t k) {
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

char bfGetBit(BloomFilter *bf, int64_t i, int *pError) {
    assert(bf);

    if (i >= bfSize(bf)) {
        if (pError) {
            *pError = 1;
        }

        return 0;
    }

    int64_t offset = i / 8;
    int64_t position = i % 8;

    if (pError) {
        *pError = 0;
    }

    char *byte = getByte(bf, offset);
    return (*byte >> position) & 0x1;
}

bool bfSetBit(BloomFilter *bf, int64_t i) {
    assert(bf);

    if (i >= bfSize(bf)) {
        return false;
    }

    int64_t offset = i / 8;
    int64_t position = i % 8;

    char *byte = getByte(bf, offset);
    int64_t mask = 1 << position;

    *byte = *byte | mask;

    return true;
}

bool bfAdd(BloomFilter *bf, void *value, int len) {
    assert(bf);
    assert(value);

    if (len <= 0) {
        return false;
    }

    int64_t size = bfSize(bf);
    int nbHashs = bfNbHashs(bf);
    uint32_t hash[4];

    for (int i = 0;i < nbHashs;i++) {
        MurmurHash3_x64_128(value, len, i, hash);

        int64_t pos = getPositionFromHash(hash, 4, size);

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

    size_t size = bfSize(bf);
    int nbHashs = bfNbHashs(bf);
    uint32_t hash[4];
    int error;

    for (int i = 0;i < nbHashs;i++) {
        MurmurHash3_x64_128(value, len, i, hash);

        size_t pos = getPositionFromHash(hash, 4, size);

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

int64_t bfSize(const BloomFilter *bf) {
    assert(bf);

    return bf->size;
}

int bfNbHashs(const BloomFilter *bf) {
    assert(bf);

    return bf->nbhashs;
}