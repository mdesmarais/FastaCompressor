#include "bloom_filter.h"
#include "de_bruijn_graph.h"
#include "fasta.h"
#include "log.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing path to a fasta file\n");
        return -1;
    }

    FILE *fp;
    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror("Unable to open file");
        return -1;
    }

    BloomFilter *bf = bfCreate(10000000, 7);

    if (!bf) {
        log_error("Unable to create a new Bloom filter");
        goto EXIT;
    }

    if (!createDBG(bf, fp, 20)) {
        log_error("Unable to fill the graph with the given file");
        goto EXIT;
    }

    FILE *out;
    if ((out = fopen("output.graph", "wb")) == NULL) {
        log_error("Unable to create the output file");
        log_error(strerror(errno));
        goto EXIT;
    }

    if (!saveDBG(bf, out)) {
        log_error("save failed");
        fclose(out);
        goto EXIT;
    }

    fclose(out);
    
    if ((out = fopen("output.pouet", "w")) == NULL) {
        log_error(strerror(errno));
        goto EXIT;
    }

    fseek(fp, 0, 0);

    if (!compressFile(bf, fp, out, 20)) {
        log_error("compression error");
        goto EXIT;
    }

    log_info("Done.");

EXIT:
    bfDelete(bf);
    fclose(fp);
    fclose(out);
    return 0;
}