#include "de_bruijn_graph.h"

#include "bloom_filter.h"
#include "getline.h"

#include <assert.h>
#include <string.h>

void createDBG(BloomFilter *bf, FILE *fp, int k) {
    assert(bf);
    assert(fp);

    char *line = NULL;
    size_t length = 0;

    ssize_t result;
    while ((result = getline(&line, &length, fp)) > 0) {
        if (*line == '>') {
            continue;
        }

        for (size_t i = 0;i < length - k + 1;i++) {
            if (!bfAdd(bf, line + i, k)) {

            }
        }
    }

    if (result < 0) {
        perror("somethink bad happened");
        return;
    }
}