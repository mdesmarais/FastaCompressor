#include "bloom_filter.h"
#include "de_bruijn_graph.h"
#include "fasta.h"
#include "log.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#define FAIL()

int main(int argc, char **argv) {
    int resultStatus = EXIT_FAILURE;

    if (argc < 2) {
        fprintf(stderr, "Missing path to a fasta file\n");
        return -1;
    }

    FILE *fp = NULL;
    FILE *fOut = NULL;
    BloomFilter *bf = NULL;

    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror("Unable to open file");
        return -1;
    }

    // Creates a new Bloom Filter with default parameters
    if ((bf = bfCreate(10000000, 7)) == NULL) {
        log_error("Unable to create a new Bloom filter");
        goto EXIT;
    }

    if (!createDBG(bf, fp, 20)) {
        log_error("Unable to fill the graph with the given file");
        goto EXIT;
    }

    gzFile graphOut = NULL;
    if ((graphOut = gzopen("output.graph", "wb9")) == NULL) {
        log_error("Unable to create the output file");
        log_error(strerror(errno));
        goto EXIT;
    }

    if (!saveDBG(bf, graphOut)) {
        log_error("save failed");
        gzclose(graphOut);
        goto EXIT;
    }

    gzclose(graphOut);
    
    if ((fOut = fopen("output.pouet", "w")) == NULL) {
        log_error(strerror(errno));
        goto EXIT;
    }

    // Lets read the file again for the compression
    fseek(fp, 0, 0);

    if (!compressFile(bf, fp, fOut, 20)) {
        log_error("compression error");
        goto EXIT;
    }

    log_info("Done.");
    resultStatus = EXIT_SUCCESS;

EXIT:
    bfDelete(bf);
    fclose(fp);
    fclose(fOut);
    return resultStatus;
}