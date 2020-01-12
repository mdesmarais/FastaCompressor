#ifndef DECOMPRESS_THREAD_H
#define DECOMPRESS_THREAD_H

#include <stdbool.h>
#include <stdio.h>

struct BloomFilter;
struct Queue;

typedef struct ThreadArgs {
    struct BloomFilter *bf;
    FILE *out;
    struct Queue *workQueue;
    struct Queue *outQueue;
    int kmerLength;
    int readLength;
} ThreadArgs;

bool decompressFileThreads(struct BloomFilter *bf, FILE *in, FILE *out, int k);

#endif // DECOMPRESS_THREAD_H