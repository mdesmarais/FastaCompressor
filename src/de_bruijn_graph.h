#ifndef DE_BRUIJN_GRAPH_H
#define DE_BRUIJN_GRAPH_H

#include <stdbool.h>
#include <stdio.h>

#include <zlib.h>

struct BloomFilter;

bool createDBG(struct BloomFilter *bf, FILE *fp, int k);

struct BloomFilter *loadDBG(gzFile fp);
bool saveDBG(struct BloomFilter *bf, gzFile fp);

#endif // DE_BRUIJN_GRAPH_H