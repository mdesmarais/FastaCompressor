
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#include "bloom_filter.h"
#include "de_bruijn_graph.h"
#include "fasta.h"
#include "log.h"
#include "string_utils.h"
#include "utils.h"

void help(const char *prog) {
    printf("Usage : %s [--graph file] [--output, -o file] compressed_file\n\n", prog);

    printf("--graph -> path to a file for loading Bloom filter\n");
    printf("--output, -o file -> path to a file for writing decompressed reads\n");
}

int main(int argc, char **argv) {
    struct option options[] = {
        { "help", no_argument, NULL, '?' },
        { "graph", required_argument, NULL, 'g' },
        { "output", required_argument, NULL, 'o' },
        { 0, 0, 0, 0 }
    };

    char graphPath[255] = { '\0' };
    char outputPath[255] = { '\0' };

    int opt;
    while ((opt = getopt_long(argc, argv, "?1:o:", options, NULL)) != -1) {
        switch(opt) {
            case '?':
                help(argv[0]);
                return EXIT_SUCCESS;

            case 'g':
                strncpy(graphPath, optarg, 255);
                break;
            
            case 'o':
                strncpy(outputPath, optarg, 255);
                break;
            
            default:
                fprintf(stderr, "Unknown option %s\n", optarg);
                return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Missing path to a compressed file\n");
        help(argv[0]);
        return EXIT_FAILURE;
    }

    char *inputFile = argv[optind];
    size_t pathLength = strlen(inputFile);

    if (pathLength > 255) {
        fprintf(stderr, "Given path is too long\n");
        return EXIT_FAILURE;
    }

    if (graphPath[0] == '\0' && pathExtension(inputFile, pathLength, "graph.gz", 8, graphPath, 255) == 0) {
        fprintf(stderr, "The given path is too long\n");
        return EXIT_FAILURE;
    }

    if (outputPath[0] == '\0' && pathExtension(inputFile, pathLength, "fasta", 5, outputPath, 255) == 0) {
        fprintf(stderr, "The given path is too long\n");
        return EXIT_FAILURE;
    }

    // Objects that have to freed before
    // the end of the program
    gzFile graphFp = NULL;
    FILE *inFp = NULL;
    FILE *outFp = NULL;
    BloomFilter *bf = NULL;

    int result = EXIT_FAILURE;

    if ((graphFp = gzopen(graphPath, "rb")) == NULL) {
        log_error("Unable to open the graph file : %s", gzFileError(graphFp));
        return EXIT_FAILURE;
    }

    log_info("Loading graph");
    if ((bf = loadDBG(graphFp)) == NULL) {
        log_error("Unable to load graph from %s", graphPath);
        goto EXIT;
    }

    gzclose(graphFp);
    graphFp = NULL;

    log_info("Done.");

    if ((inFp = fopen(inputFile, "r")) == NULL) {
        log_error("Unable to open %s", inputFile);
        log_error(strerror(errno));
        goto EXIT;
    }

    if ((outFp = fopen(outputPath, "w")) == NULL) {
        log_error("Unable to open %s", outputPath);
        log_error(strerror(errno));
        goto EXIT;
    }

    log_info("Decompressing file");
    if (!decompressFile(bf, inFp, outFp, 20)) {
        log_error("coup dur");
        goto EXIT;
    }

    log_info("Done.");

    result = EXIT_SUCCESS;

EXIT:
    if (graphFp) {
        gzclose(graphFp);
    }

    if (inFp) {
        fclose(inFp);
    }
    bfDelete(bf);

    return result;
}