#include "de_bruijn_graph.h"

#include "bloom_filter.h"
#include "getline.h"
#include "log.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
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

struct BloomFilter *loadDBG(FILE *fp) {
    assert(fp);

    int64_t size = 0;
    if (fread(&size, sizeof(size), 1, fp) != 1) {
        if (!feof(fp)) {
            log_error("Unable to read the size of the graph : %s\n", strerror(errno));
        }
        else {
            log_error("size error");
        }
        return NULL;
    }

    int8_t nbHashs = 0;
    if (fread(&nbHashs, 1, 1, fp) != 1) {
        if (!feof(fp)) {
            log_error("Unable to read the number of hashs : %s\n", strerror(errno));
        }
        else {
            log_error("nbhashs error");
        }
        return NULL;
    }

    int8_t *bits = malloc(sizeof(*bits) * size);

    if (!bits) {
        log_error("Unable to allocate a buffer of size %I64d\n", size);
        return NULL;
    }

    if (fread(bits, 1, size, fp) != size) {
        if (!feof(fp)) {
            log_error("Unable to read the content of the Bloom filter\n");
        }
        free(bits);
        return NULL;
    }

    BloomFilter *bf = bfCreate(size, nbHashs);

    if (!bf) {
        log_error("Unable to create a new Bloom filter");
        free(bits);
        return NULL;
    }

    bfFill(bf, bits);

    free(bits);
    return bf;
}

bool saveDBG(BloomFilter *bf, FILE *fp) {
    assert(bf);
    assert(fp);

    // @TODO check endianness

    if (fwrite(&bf->size, sizeof(bf->size), 1, fp) < 1) {
        goto ERROR;
    }

    if (fwrite(&bf->nbhashs, sizeof(bf->nbhashs), 1, fp) < 1) {
        goto ERROR;
    }

    if (fwrite(bf->data, sizeof(*bf->data), bf->size, fp) < bf->size) {
        goto ERROR;
    }

    return true;
ERROR:
    log_error("saveDBG has failed : %s", strerror(errno));
    return false;
}