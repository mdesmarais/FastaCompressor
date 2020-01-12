#include "decompress_thread.h"

#include "bloom_filter.h"
#include "fasta.h"
#include "log.h"
#include "queue.h"
#include "getline.h"
#include "vector.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
 * \brief Decompresses reads from a work queue
 * 
 * The given args should be a ThreadArgs structure.
 * 
 * @param voidArgs a pointer to a ThreadArgs structure
 * @return not used for now
 */
static void *decompressionWorker(void *voidArgs) {
    assert(voidArgs);

    ThreadArgs *args = voidArgs;

    Vector *branchings = vectorCreate(10, 1);

    if (!branchings) {
        log_error("oulala");
        return NULL;
    }

    char *buffer = NULL;
    char *read = NULL;

    buffer = malloc(args->workQueue->elemSize);

    if (!buffer) {
        log_error("Unable to allocate a new buffer of size %ld", args->workQueue->elemSize);
        goto EXIT;
    }

    read = malloc(args->readLength);

    if (!read) {
        log_error("Unable to allocate a new buffer of size %d", args->readLength);
        goto EXIT;
    }

    while (true) {
        vectorClear(branchings);

        if (!queuePop(args->workQueue, buffer)) {
            log_error("Unable to pop an element from the work queue");
            goto EXIT;
        }

        // Delimiter that indicates the end of the thread
        if (*buffer == '\0') {
            queuePush(args->workQueue, buffer);
            break;
        }

        int nbBranchings;
        if ((nbBranchings = extractBranchings(branchings, buffer)) < 0) {
            log_error("Unable to extract branchings");
            goto EXIT;
        }

        if (!decompressRead(args->bf, branchings, read, args->readLength, buffer, args->kmerLength)) {
            log_error("Unable to decompress a read");
            goto EXIT;
        }

        // Inserts the decompressed read into the
        // output queue
        if (!queuePush(args->outQueue, read)) {
            log_error("Unable to push a read into the out queue");
            goto EXIT;
        }
    }

EXIT:
    free(buffer);
    free(read);
    vectorDelete(branchings);

    return NULL;
}

/**
 * \brief Writes lines from a queue into an output file
 * 
 * This function should be executed by only one thread.
 * The given args should be a ThreadArgs structure that
 * contains the destination file and the queue.
 * 
 * @param voidArgs a pointer to a ThreadArgs structure
 * @return not used for now
 */
static void *outputWorker(void *voidArgs) {
    assert(voidArgs);

    ThreadArgs *args = voidArgs;
    Queue *queue = args->outQueue;
    int readLength = args->readLength;

    char *buffer = malloc(queue->elemSize + 1);
    buffer[queue->elemSize] = '\0';

    long readIndex = 1;

    while (true) {
        if (!queuePop(queue, buffer)) {
            log_error("Queue pop error");
            break;
        }

        // Delimiter that indicates the end of the thread
        if (*buffer == '\0') {
            log_error("output worker Stop at %ld", readIndex);
            break;
        }

        fprintf(args->out, ">read %ld\n%s\n", readIndex, buffer);

        readIndex++;
    }

    free(buffer);

    return voidArgs;
}

bool decompressFileThreads(BloomFilter *bf, FILE *in, FILE *out, int k) {
    assert(bf);
    assert(in);
    assert(out);
    
    if (k <= 0) {
        return false;
    }

    // Gets read length, it should be the first line of the file
    int readLength = 0;
    if (!fscanf(in, "%d", &readLength)) {
        log_error("Unable to read the length of each reads");
        return false;
    }

    log_debug("Read length: %d", readLength);

    // Skips characters until the new line
    int c;
    while ((c = getc(in)) != EOF && c != '\n') { }

    // A compressed read follows this format : "first_kmer branchings"
    size_t lineSize = readLength * 2 + 2;
    
    // Queue for storing compressed reads from the main thread
    Queue *workQueue = queueCreate(800, lineSize);

    if (!workQueue) {
        log_error("Unable to create a new work queue");
        return false;
    }

    // Queue for storing decompressed reads from workers
    Queue *outQueue = queueCreate(400, readLength);

    if (!outQueue) {
        log_error("Unable to create a new out queue");
        queueDelete(outQueue);
        return false;
    }

    // Threads arguments initialization
    ThreadArgs args;
    args.bf = bf;
    args.out = out;
    args.outQueue = outQueue;
    args.readLength = readLength;
    args.kmerLength = k;
    args.workQueue = workQueue;

    // @TODO should be a function parameter
    int nbthreads = 4;
    pthread_t threads[nbthreads];

    // Holds a line from the input file (a compressed read)
    char *line = NULL;

    // decompressFileThreads result
    bool result = false;

    // Creates file writer thread
    if (pthread_create(threads, NULL, outputWorker, &args) != 0) {
        log_error("Thread creation error");
        goto EXIT;
    }

    // Creates decompression threads
    for (int i = 1;i < nbthreads;i++) {
        if (pthread_create(threads + i, NULL, decompressionWorker, &args) != 0) {
            log_error("Thread creation error");
            goto EXIT;
        }
    }

    line = malloc(lineSize);

    if (!line) {
        log_error("Unable to allocate a buffer of size %ld", lineSize);
        goto EXIT;
    }

    // Input file reading
    while (fgets(line, lineSize, in) != NULL) {
        if (!queuePush(workQueue, line)) {
            log_error("Queue push error");
            goto EXIT;
        }
    }

    if (!feof(in)) {
        log_error("File error : %s", strerror(errno));
        goto EXIT;
    }

    // Inserts a marker into the work queue in order to
    // inform threads that there is no more in coming reads
    *line = '\0';
    if (!queuePush(workQueue, line)) {
        log_error("Delimiter push error");
    }

    // Waits for the workers
    for (int i = 1;i < nbthreads;i++) {
        pthread_join(threads[i], NULL);
    }

    // Inserts a marker into the output queue in order to
    // inform the first thread that there is no more in coming lines
    *line = '\0';
    queuePush(outQueue, line);
    pthread_join(threads[0], NULL);

    result = true;

EXIT:
    free(line);
    
    for (int i = 0;i < nbthreads;i++) {
        pthread_join(threads[i], NULL);
    }

    queueDelete(workQueue);
    queueDelete(outQueue);

    return result;
}