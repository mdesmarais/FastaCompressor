#include "bloom_filter.h"
#include "de_bruijn_graph.h"
#include "fasta.h"
#include "log.h"
#include "string_utils.h"

#include <errno.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

void help(char *prog) {
    printf("Usage: %s [--output output_file] [--graph output_graph_file] [--kmer-size size] [--bloom-size size] [--bloom-hash hash] fasta_file\n\n", prog);

    printf("--output output_file -> path to a file for storing compressed reads\n");
    printf("--graph output_graph_file -> path to a file for storing the graph\n");
    printf("--kmer-size size -> size of a kmer\n");
    printf("--bloom-size size -> size of the Bloom filter\n");
    printf("--bloom-hash hash -> number of hash functions\n\n");
}

int main(int argc, char **argv) {
    struct option options[] = {
        { "help", no_argument, NULL, '?' },
        { "output", required_argument, NULL, 1 },
        { "graph", required_argument, NULL, 2 },
        { "kmer-size", required_argument, NULL, 3 },
        { "bloom-size", required_argument, NULL, 4 },
        { "bloom-hash", required_argument, NULL, 5 },
        { 0, 0, 0, 0 }
    };

    char outputFile[255] = { '\0' };
    char graphOutputFile[255] = { '\0' };

    int kmerSize = 20;
    int64_t bfSize = 10000000;
    int bfHash = 7;

    int opt;
    while ((opt = getopt_long(argc, argv, "?1:2:3:4:5:", options, NULL)) != -1) {
        switch (opt) {
            case '?':
                help(argv[0]);
                break;

            case 1:
                strncpy(outputFile, optarg, 255);
                break;

            case 2:
                strncpy(graphOutputFile, optarg, 255);
                break;

            case 3: {
                int8_t value = atoi8(optarg);

                if (value <= 0) {
                    fprintf(stderr, "Invalid kmer size\n");
                    return EXIT_FAILURE;
                }

                kmerSize = value;
                break;
            }

            case 4: {
                int64_t value = atoi64(optarg);

                if (value <= 0) {
                    fprintf(stderr, "Invalid Bloom filter size\n");
                    return EXIT_FAILURE;
                }

                bfSize = value;
                break;
            }

            case 5: {
                int8_t value = atoi8(optarg);

                if (value <= 0) {
                    fprintf(stderr, "Invalid Bloom hashs number\n");
                    return EXIT_FAILURE;
                }

                bfHash = value;
                break;
            }
            
            default:
                fprintf(stderr, "Unknown option\n\n");
                help(argv[0]);
                break;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Missing path to a fasta file\n\n");
        help(argv[0]);
        return EXIT_FAILURE;
    }

    char *inputFilePath = argv[optind];
    size_t pathLen = strlen(inputFilePath);

    if (pathLen > 255) {
        fprintf(stderr, "The given path is too long\n");
        return EXIT_FAILURE;
    }

    // Generates an output filename if the user did not specified one
    if (*outputFile == '\0' && pathExtension(inputFilePath, pathLen, "comp", 4, outputFile, 255) == 0) {
        fprintf(stderr, "The given path is too long\n");
        return EXIT_FAILURE;
    }

    // Generates an output graph filename if the user did not specified one
    if (*graphOutputFile == '\0' && pathExtension(inputFilePath, pathLen, "graph.gz", 8, graphOutputFile, 255) == 0) {
        fprintf(stderr, "The given path is too long\n");
        return EXIT_FAILURE;
    }

    // Informs the user of paths and parameters that will be used
    log_info("Compressed fasta path : %s", outputFile);
    log_info("Graph path : %s", graphOutputFile);
    log_info("Parameters : kmer-size=%d filter-size=%d filter-hash=%d", kmerSize, bfSize, bfHash);

    int resultStatus = EXIT_FAILURE;

    // Variables that have to the freed
    // before the end of the program
    FILE *inFp = NULL;
    FILE *outFp = NULL;
    BloomFilter *bf = NULL;

    if ((inFp = fopen(inputFilePath, "r")) == NULL) {
        perror("Unable to open input file");
        return EXIT_FAILURE;
    }

    // Creates a new Bloom Filter with default parameters
    if ((bf = bfCreate(bfSize, bfHash)) == NULL) {
        log_error("Unable to create a new Bloom filter");
        goto EXIT;
    }

    log_info("Creating De Bruijn graph");
    if (!createDBG(bf, inFp, kmerSize)) {
        log_error("Unable to fill the graph with the given file");
        goto EXIT;
    }
    log_info("Done.");

    gzFile graphOut = NULL;
    if ((graphOut = gzopen(graphOutputFile, "wb9")) == NULL) {
        log_error("Unable to create the output file");
        log_error(strerror(errno));
        goto EXIT;
    }

    log_info("Saving graph to ...");
    if (!saveDBG(bf, graphOut)) {
        log_error("save failed");
        gzclose(graphOut);
        goto EXIT;
    }
    log_info("Done.");

    gzclose(graphOut);

    // Lets read the file again
    fseek(inFp, 0, SEEK_SET);
    
    if ((outFp = fopen(outputFile, "w")) == NULL) {
        log_error(strerror(errno));
        goto EXIT;
    }

    log_info("Compressing reads");
    if (!compressFile(bf, inFp, outFp, kmerSize)) {
        log_error("compression error");
        goto EXIT;
    }

    log_info("Done.");
    resultStatus = EXIT_SUCCESS;

EXIT:
    bfDelete(bf);
    if (inFp) {
        fclose(inFp);
    }
    if (outFp) {
        fclose(outFp);
    }

    return resultStatus;
}