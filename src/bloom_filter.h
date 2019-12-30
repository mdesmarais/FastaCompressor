#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct BloomFilter {
    int8_t *data;
    int64_t size;
    int8_t nbhashs;
} BloomFilter;

BloomFilter *bfCreate(int64_t n, int8_t k);
void bfDelete(BloomFilter *bf);

void bfFill(BloomFilter *bf, int8_t *bits);

char bfGetBit(BloomFilter *bf, int64_t i, int *pError);
bool bfSetBit(BloomFilter *bf, int64_t i);

bool bfAdd(BloomFilter *bf, void *value, int len);
bool bfContains(BloomFilter *bf, void *value, int len);

int64_t bfSize(const BloomFilter *bf);
int bfNbHashs(const BloomFilter *bf);

#endif // BLOOM_FILTER_H
