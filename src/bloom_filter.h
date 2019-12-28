#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H
#include <stdbool.h>
#include <stddef.h>

typedef struct BloomFilter {
    char *data;
    size_t size;
    int nbhashs;
} BloomFilter;

BloomFilter *bfCreate(size_t n, int k);
void bfDelete(BloomFilter *bf);

char bfGetBit(BloomFilter *bf, size_t i, int *pError);
bool bfSetBit(BloomFilter *bf, size_t i);

bool bfAdd(BloomFilter *bf, void *value, int len);
bool bfContains(BloomFilter *bf, void *value, int len);

size_t bfSize(const BloomFilter *bf);
int bfNbHashs(const BloomFilter *bf);

#endif // BLOOM_FILTER_H
