#ifndef FASTA_H
#define FASTA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct BloomFilter;
struct Vector;

bool compressFile(struct BloomFilter *bf, FILE *in, FILE *out, int k);
bool computeBranchings(struct BloomFilter *bf, struct Vector *v, char *seq, size_t len, int k);
bool decompressRead(struct BloomFilter *bf, struct Vector *branchings, char *read, int readLength, const char *firstKmer, int k);

#endif // FASTA_H