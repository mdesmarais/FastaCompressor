#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct BloomFilter {
    char *data;
    long size;
    int8_t nbhashs;
} BloomFilter;

#define bfNbHashs(bf) ((bf)->nbhashs)
#define bfSize(bf) ((bf)->size)

BloomFilter *bfCreate(long n, int8_t k);
void bfDelete(BloomFilter *bf);

void bfFill(BloomFilter *bf, int8_t *bits);

char bfGetBit(BloomFilter *bf, long long i, int *pError);
bool bfSetBit(BloomFilter *bf, long long i);

bool bfAdd(BloomFilter *bf, void *value, int len);
bool bfContains(BloomFilter *bf, void *value, int len);

#endif // BLOOM_FILTER_H
