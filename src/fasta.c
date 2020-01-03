#include "fasta.h"

#include "bloom_filter.h"
#include "getline.h"
#include "log.h"
#include "utils.h"
#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

bool compressFile(BloomFilter *bf, FILE *in, FILE *out, int k) {
    assert(bf);
    assert(in);
    assert(out);

    if (k <= 0) {
        return false;
    }

    Vector *v = vectorCreate(100, 1);

    if (!v) {
        log_error("Unable to create a new vector");
        return false;
    }

    char *line = NULL;
    size_t size = 0;
    bool firstLine = true;

    ssize_t result;
    while ((result = getline(&line, &size, in)) > 0) {
        if (*line == '>') {
            continue;
        }

        if (k > result) {
            return false;
        }

        if (firstLine) {
            firstLine = false;
            fprintf(out, "%ld\n", result - 1);
        }

        vectorClear(v);
        if (!computeBranchings(bf, v, line, result, k)) {
            log_error("branchings computation error");
            break;
        }

        fprintf(out, "%.*s %.*s\n", k, line, (int) vectorSize(v), (char*) vectorRawValues(v));
    }

    free(line);
    vectorDelete(v);

    return true;
}

bool computeBranchings(BloomFilter *bf, Vector *v, char *seq, size_t len, int k) {
    assert(bf);
    assert(v);
    assert(seq);

    if (k <= 0) {
        return false;
    }

    char neighbors[4];

    for (size_t i = 0;i < len - k - 1;i++) {
        int nbNeighbors = findNeighbors(bf, seq + i, k, neighbors);

        if (nbNeighbors < 0) {
            return false;
        }

        if (nbNeighbors == 0) {
            break;
        }
        else if (nbNeighbors > 1 && !vectorPush(v, seq + i + k)) {
            log_error("vector push error");
            return false;
        }
    }

    return true;
}

bool decompressFile(BloomFilter *bf, FILE *in, FILE *out, int k) {
    assert(bf);
    assert(in);
    assert(out);

    if (k <= 0) {
        return false;
    }

    bool result = false;
    char *line = NULL;
    size_t size = 0;
    char *read = NULL;
    Vector *branchings = NULL;

    ssize_t lineLength = getline(&line, &size, in);

    if (lineLength <= 0) {
        log_error("Unable to read the length of each read");
        goto EXIT;
    }

    int readLength = 0;

    if (sscanf(line, "%d", &readLength) != 1) {
        log_error("Unable to extract the length of each read");
        goto EXIT;
    }

    if (readLength <= 0) {
        log_error("Invalid read length %d", readLength);
        goto EXIT;
    }

    log_debug("Read length : %d", readLength);

    read = malloc(readLength + 1);
    branchings = vectorCreate(readLength - k, 1);

    if (!read) {
        log_error("Allocation error");
        goto EXIT;
    }

    if (!branchings) {
        log_error("Unable to create a new vector");
        goto EXIT;
    }

    read[readLength] = '\0';

    size_t readIndex = 0;
    while ((lineLength = getline(&line, &size, in)) > 0) {
        vectorClear(branchings);

        int nbBranchings;
        if ((nbBranchings = extractBranchings(branchings, line)) < 0) {
            log_error("Unable to extract branchings");
            goto EXIT;
        }

        // @TODO check that the read length equals k

        if (!decompressRead(bf, branchings, read, readLength, line, k)) {
            log_error("Decompression error");
            goto EXIT;
        }

        //fwrite(read, readLength + 1, 1, out);
        fprintf(out, ">read %ld\n%s\n", readIndex, read);
        readIndex++;
    }

    result = true;

EXIT:
    free(line);
    free(read);
    vectorDelete(branchings);

    return result;
}

bool decompressRead(BloomFilter *bf, Vector *branchings, char *read, int readLength, const char *firstKmer, int k) {
    assert(bf);
    assert(branchings);
    assert(read);
    assert(firstKmer);

    if (readLength <= 0 || k <= 0) {
        return false;
    }

    char *kmer = malloc(k);
    bool result = false;

    if (!kmer) {
        log_error("Allocation failed");
        return false;
    }

    memcpy(kmer, firstKmer, k);
    memcpy(read, kmer, k);

    char neighbors[4];
    int nextBranching = 0;

    for (int i = 0;i < readLength - k;i++) {
        int nbNeighbors = findNeighbors(bf, kmer, k, neighbors);
        int neighborIndex = -1;

        if (nbNeighbors > 1) {
            char *pBranching = NULL;

            if ((pBranching = vectorAt(branchings, nextBranching)) == NULL) {
                log_error("Unable to get branching at index %d, vector size=%ld", nextBranching, vectorSize(branchings));
                goto EXIT;
            }

            // Checks if the current branching corresponds to one of the kmer neighbors
            for (int j = 0;j < nbNeighbors && neighborIndex == -1;j++) {
                if (*pBranching == neighbors[j]) {
                    neighborIndex = j;
                }
            }

            if (neighborIndex == -1) {
                log_error("Branching error, index=%ld\npartial read=%.*s\navailable neighbors : %.*s", i, i, read, nbNeighbors, neighbors);
                return false;
            }

            nextBranching++;
        }
        else if (nbNeighbors == 1) {
            neighborIndex = 0;
        }
        else if (i == readLength - k) {
            // It was the last kmer
            break;
        }
        else {
            // Error
            log_error("Kmer without neighbors at %d, kmer=%.*s", i, k, firstKmer);
            goto EXIT;
        }

        memcpy(kmer, kmer + 1, k - 1);
        kmer[k - 1] = neighbors[neighborIndex];
        read[i + k] = neighbors[neighborIndex];
    }

    result = true;

EXIT:
    free(kmer);

    return result;
}

int extractBranchings(Vector *branchings, const char *line) {
    assert(branchings);
    assert(line);

    char *seqEnd = strchr(line, ' ');

    if (!seqEnd) {
        return 0;
    }

    char *nextBranching = seqEnd + 1;
    while (*nextBranching != '\0' && *nextBranching != '\n') {
        if (!vectorPush(branchings, nextBranching)) {
            log_error("Unable to push a new branching into the vector");
            return -1;
        }

        nextBranching++;
    }

    return nextBranching - seqEnd - 1;
}