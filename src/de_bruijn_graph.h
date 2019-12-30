#ifndef DE_BRUIJN_GRAPH_H
#define DE_BRUIJN_GRAPH_H

#include <stdbool.h>
#include <stdio.h>

struct BloomFilter;

void createDBG(struct BloomFilter *bf, FILE *fp, int k);

struct BloomFilter *loadDBG(FILE *fp);
bool saveDBG(struct BloomFilter *bf, FILE *fp);

#endif // DE_BRUIJN_GRAPH_H