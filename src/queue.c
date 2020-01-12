#include "queue.h"

#include "log.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

Queue *queueCreate(size_t capacity, size_t elemSize) {
    Queue *queue = malloc(sizeof(*queue));

    if (!queue) {
        return NULL;
    }

    queue->data = malloc(capacity * elemSize);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    int r1 = pthread_mutex_init(&queue->lock, &attr);
    int r2 = pthread_cond_init(&queue->elemAvailable, NULL);
    int r3 = pthread_cond_init(&queue->slotAvailable, NULL);

    if (r1 != 0) {
        log_error("Mutex init error");
        goto ERROR;
    }

    if (r2 != 0 || r3 != 0) {
        log_error("Cond init error");
        goto ERROR;
    }

    if (!queue->data) {
        goto ERROR;
    }

    queue->elemSize = elemSize;
    queue->capacity = capacity;
    
    queueClear(queue);

    return queue;

ERROR:
    free(queue->data);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->elemAvailable);
    pthread_cond_destroy(&queue->slotAvailable);
    free(queue);
    return NULL;
}

void queueDelete(Queue *queue) {
    if (queue) {
        pthread_mutex_destroy(&queue->lock);
        pthread_cond_destroy(&queue->elemAvailable);
        pthread_cond_destroy(&queue->slotAvailable);
        free(queue->data);
        free(queue);
    }
}

void queueClear(Queue *queue) {
    assert(queue);

    queue->head = 0;
    queue->tail = 0;
    queue->full = false;

    queue->emptyWaiters = 0;
    queue->fullWaiters = 0;
}

bool queuePop(Queue *queue, void *value) {
    assert(queue);
    assert(value);

    if (pthread_mutex_lock(&queue->lock) != 0) {
        log_error("Mutex lock error");
        return false;
    }

    while (queueEmpty(queue)) {
        queue->emptyWaiters++;
        if (pthread_cond_wait(&queue->elemAvailable, &queue->lock) != 0) {
            log_error("cond wait error");
            pthread_mutex_unlock(&queue->lock);
            return false;
        }
        queue->emptyWaiters--;
    }

    memcpy(value, queue->data + queue->elemSize * queue->tail, queue->elemSize);

    queue->full = false;
    queue->tail = (queue->tail + 1) % queue->capacity;

    if (queue->fullWaiters && pthread_cond_signal(&queue->slotAvailable) != 0) {
        log_error("Mutex not full signal error");
        pthread_mutex_unlock(&queue->lock);
        return false;
    }

    pthread_mutex_unlock(&queue->lock);

    return true;
}

bool queuePush(Queue *queue, void *value) {
    assert(queue);
    assert(value);

    if (pthread_mutex_lock(&queue->lock) != 0) {
        log_error("Mutex lock error");
        return false;
    }

    while (queueFull(queue)) {
        queue->fullWaiters++;
        if (pthread_cond_wait(&queue->slotAvailable, &queue->lock) != 0) {
            log_error("cond wait error");
            pthread_mutex_unlock(&queue->lock);
            return false;
        }
        queue->emptyWaiters--;
    }

    memcpy(queue->data + queue->elemSize * queue->head, value, queue->elemSize);

    queue->head = (queue->head + 1) % queue->capacity;
    queue->full = queue->head == queue->tail;

    if (queue->emptyWaiters && pthread_cond_signal(&queue->elemAvailable) != 0) {
        log_error("Cond signal error");
        pthread_mutex_unlock(&queue->lock);
        return false;
    }

    pthread_mutex_unlock(&queue->lock);

    return true;
}