#ifndef QUEUE_H
#define QUEUE_H
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct Queue {
    void *data;
    size_t elemSize;
    size_t capacity;
    size_t head;
    size_t tail;
    bool full;
    pthread_mutex_t lock;
    pthread_cond_t elemAvailable;
} Queue;

#define queueEmpty(queue) (!(queue)->full && (queue)->head == (queue)->tail)
#define queueFull(queue) ((queue)->full)

/**
 * \brief Creates a new thread safe queue with the given capacity
 * 
 * If an allocation error occures then NULL will be returned.
 * 
 * @param capacity number of elements that the queue could store
 * @param elemSize size of each element
 * @return a pointer to an heap allocated Queue structure
 */
Queue *queueCreate(size_t capacity, size_t elemSize);

/**
 * \brief Frees the allocated memory of the queue
 * 
 * The given pointer must no be used after the call
 * to this function.
 * 
 * @param queue a pointer to an heap allocated Queue structure
 */
void queueDelete(Queue *queue);

/**
 * \brief Clears the content of the queue
 * 
 * @param queue a pointer to a Queue structure
 */
void queueClear(Queue *queue);

/**
 * \briefs Gets the tail of the list
 * 
 * If the queue is empty then the current thread
 * will wait for an available value.
 * 
 * The given destination buffer must be big enough
 * to store the value.
 * 
 * @param queue a pointer to a Queue structure
 * @param value destination of the tail
 * @return true if no error occured, otherwise false
 */
bool queuePop(Queue *queue, void *value);

/**
 * \briefs Inserts a value at the head of the list
 * 
 * If the queue is full then the current thread
 * will wait for a free slot.
 * 
 * Only one thread should call this function.
 * 
 * If the internal mutex can not be locked / unlocked
 * then false will be returned.
 * 
 * @param queue a pointer to a Queue structure
 * @param value pointer to the value to add
 * @return true if no error occured, otherwise false
 */
bool queuePush(Queue *queue, void *value);

#endif // QUEUE_H