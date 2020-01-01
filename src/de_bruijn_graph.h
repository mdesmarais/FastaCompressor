#ifndef DE_BRUIJN_GRAPH_H
#define DE_BRUIJN_GRAPH_H

#include <stdbool.h>
#include <stdio.h>

#include <zlib.h>

struct BloomFilter;

bool createDBG(struct BloomFilter *bf, FILE *fp, int k);

/**
 * Inserts the canonical kmer form into the Bloom filter
 * 
 * If the kmer length k is negative or equals 0 then false will be returned.
 * The given kmer will be modified to store its canonical form.
 * 
 * @param bf a pointer to a Bloom filter structure, it will store the kmer
 * @param kmer kmer to store
 * @param k length of the kmer
 * @return true if the insertion succeed, false otherwise
 */
bool insertKmer(struct BloomFilter *bf, char *kmer, int k);

struct BloomFilter *loadDBG(gzFile fp);
bool saveDBG(struct BloomFilter *bf, gzFile fp);

#endif // DE_BRUIJN_GRAPH_H