#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Vector {
    void *values;
    size_t capacity;
    size_t size;
    size_t elemSize;
} Vector;

/**
 * \brief Creates a new vector with an initial capacity
 * 
 * This function will return NULL if an allocation error occured.
 * 
 * @param initialCapacity initial capacity of the vector
 * @param elemSize size of elements that the vector will store
 * @return a pointer to an allocated Vector structure or NULL
 */
Vector *vectorCreate(size_t initialCapacity, size_t elemSize);

/**
 * \brief Frees the memory allocated for the given vector
 * 
 * @param v pointer to a Vector structure to free
 */
void vectorDelete(Vector *v);

/**
 * \brief Removes all values from the given vector
 * 
 * The vector wont be resized, only its size will be
 * changed to 0.
 * 
 * @param v pointer to a Vector structure
 */
void vectorClear(Vector *v);

/**
 * \brief Increases the capacity of the given vector
 * 
 * The new capacity must be greater than the previous one.
 * If it is not the case then false will be returned.
 * 
 * @param v pointer to a Vector structure
 * @param newCapacity new capacity of the vector
 * @return true if the vector has been correctly resized, otherwise fasse
 */
bool vectorResize(Vector *v, size_t newCapacity);

/**
 * \brief Inserts a new value into the vector
 * 
 * This value must have the same size as the one
 * given at the vector creation.
 * 
 * If the vector is not big enough to store a new value,
 * then it will be resized.
 * If the resize step failed, then false will be returned.
 * 
 * @param v pointer to a Vector structure
 * @param value the new value to add
 * @return true if the given value was correctly inserted, otherwise false
 */
bool vectorPush(Vector *v, void *value);

/**
 * \brief Returns the address of the value at the position i
 * 
 * If i is greater or equal to the size of the vector then
 * NULL will be returned.
 * 
 * @param v a pointer to a Vector structure
 * @param i index of the value
 * @return pointer to the value at position i
 */
void *vectorAt(Vector *v, size_t i);

/**
 * \brief Gets the capacity of the vector
 */
size_t vectorCapacity(const Vector *v);

/**
 * \brief Gets the size of the vector
 */
size_t vectorSize(const Vector *v);

/**
 * \brief Gets the internal array used by the vector
 */
void *vectorRawValues(const Vector *v);

#endif // VECTOR_H