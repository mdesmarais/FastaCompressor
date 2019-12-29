#ifndef DE_BRUIJN_GRAPH_H
#define DE_BRUIJN_GRAPH_H

#include <stdio.h>

struct BloomFilter;

void createDBG(struct BloomFilter *bf, FILE *fp, int k);

#endif // DE_BRUIJN_GRAPH_H