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

Vector *vectorCreate(size_t initialCapacity, size_t elemSize);
void vectorDelete(Vector *v);
void vectorClear(Vector *v);

bool vectorResize(Vector *v, size_t newCapacity);

bool vectorPush(Vector *v, void *value);
void *vectorAt(Vector *v, size_t i);

size_t vectorCapacity(const Vector *v);
size_t vectorSize(const Vector *v);
void *vectorRawValues(const Vector *v);

#endif // VECTOR_H