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

typedef struct ThreadArgs {
    struct BloomFilter *bf;
    FILE *out;
    Queue *workQueue;
    Queue *outQueue;
    int kmerLength;
    int readLength;
} ThreadArgs;

typedef struct CompressedRead {
    long id;
    char *read;
} CompressedRead;

typedef struct DecompressedRead {
    long id;
    char *read;
} DecompressedRead;

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

    int readLength = args->readLength;

    CompressedRead cr;
    DecompressedRead dr;
    Queue *queue = args->workQueue;

    while (true) {
        vectorClear(branchings);

        if (!queuePop(queue, &cr)) {
            log_error("Unable to pop an element from the work queue");
            goto EXIT;
        }

        // Delimiter that indicates the end of the thread
        if (cr.id == -1) {
            queuePush(queue, &cr);
            log_debug("Stop worker at %ld", cr.id);
            break;
        }

        int nbBranchings;
        if ((nbBranchings = extractBranchings(branchings, cr.read)) < 0) {
            log_error("Unable to extract branchings");
            goto EXIT;
        }

        dr.id = cr.id;
        dr.read = malloc(sizeof(*dr.read) * (readLength + 1));

        if (!dr.read) {
            log_error("Unable to allocate a buffer of length %d", readLength);
            break;
        }

        dr.read[readLength] = '\0';
        if (!decompressRead(args->bf, branchings, dr.read, readLength, cr.read, args->kmerLength)) {
            log_error("Unable to decompress a read");
            goto EXIT;
        }

        // Inserts the decompressed read into the
        // output queue
        if (!queuePush(args->outQueue, &dr)) {
            log_error("Unable to push a read into the out queue");
            goto EXIT;
        }

        free(cr.read);
        cr.read = NULL;

        dr.read = NULL;
    }

EXIT:
    free(cr.read);
    free(dr.read);
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

    DecompressedRead dc;
    dc.read = NULL;

    while (true) {
        if (!queuePop(queue, &dc)) {
            log_error("Queue pop error");
            break;
        }

        // Delimiter that indicates the end of the thread
        if (dc.id == -1) {
            log_debug("output worker Stop at %ld", dc.id);
            break;
        }

        fprintf(args->out, ">read %ld\n%s\n", dc.id, dc.read);

        free(dc.read);
        dc.read = NULL;
    }

    free(dc.read);

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
    // Stores a space and a null character
    size_t lineSize = readLength * 2 + 2;
    
    // Queue for storing compressed reads from the main thread
    Queue *workQueue = queueCreate(800, sizeof(CompressedRead));

    if (!workQueue) {
        log_error("Unable to create a new work queue");
        return false;
    }

    // Queue for storing decompressed reads from workers
    Queue *outQueue = queueCreate(400, sizeof(DecompressedRead));

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
    int nbThreads = 4;
    pthread_t threads[nbThreads];
    bool stoppedThreads[nbThreads];

    CompressedRead cr;
    cr.read = NULL;

    // decompressFileThreads result
    bool result = false;

    // Creates decompression threads
    for (int i = 0;i < nbThreads;i++) {
        void *(*target)(void*) = (i == 0) ? outputWorker : decompressionWorker;

        if (pthread_create(threads + i, NULL, target, &args) != 0) {
            log_error("Thread creation error");
            goto EXIT;
        }
    }

    // Input file reading
    cr.id = 0;
    while (true) {
        cr.read = malloc(lineSize);

        if (!cr.read) {
            log_error("Unable to allocate a buffer of size %ld", lineSize);
            goto EXIT;
        }

        if (!fgets(cr.read, lineSize, in)) {
            break;
        }
        
        int c = *cr.read;

        if (c == ' ' || c == '\0' || c == '\n') {
            break;
        }

        if (!queuePush(workQueue, &cr)) {
            log_error("Queue push error");
            goto EXIT;
        }

        cr.id++;
        cr.read = NULL;
    }

    free(cr.read);

    if (!feof(in)) {
        log_error("File error : %s", strerror(errno));
    }

    // Inserts a marker into the work queue in order to
    // inform threads that there is no more in coming reads
    cr.id = -1;
    cr.read = NULL;
    if (!queuePush(workQueue, &cr)) {
        log_error("Delimiter push error");
    }

    // Waits for the workers
    for (int i = 1;i < nbThreads;i++) {
        pthread_join(threads[i], NULL);
        stoppedThreads[i] = true;
    }

    // Inserts a marker into the output queue in order to
    // inform the first thread that there is no more in coming lines
    DecompressedRead dr;
    dr.id = -1;
    dr.read = NULL;
    queuePush(outQueue, &dr);

    result = true;

EXIT:
    free(cr.read);
    
    for (int i = 0;i < nbThreads;i++) {
        if (!stoppedThreads[i]) {
            pthread_join(threads[i], NULL);
        }
    }

    queueDelete(workQueue);
    queueDelete(outQueue);

    return result;
}