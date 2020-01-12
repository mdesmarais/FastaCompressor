#ifndef DECOMPRESS_THREAD_H
#define DECOMPRESS_THREAD_H

#include <stdbool.h>
#include <stdio.h>

struct BloomFilter;

bool decompressFileThreads(struct BloomFilter *bf, FILE *in, FILE *out, int k);

#endif // DECOMPRESS_THREAD_H