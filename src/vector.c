#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

Vector *vectorCreate(size_t initialCapacity, size_t elemSize) {
    Vector *v = malloc(sizeof(*v));

    if (!v) {
        return NULL;
    }

    void *values = malloc(initialCapacity * elemSize);

    if (initialCapacity > 0 && !values) {
        free(v);
        return NULL;
    }

    v->values = values;
    v->capacity = initialCapacity;
    v->size = 0;
    v->elemSize = elemSize;

    return v;
}

void vectorDelete(Vector *v) {
    if (v) {
        free(v->values);
        free(v);
    }
}

void vectorClear(Vector *v) {
    assert(v);

    v->size = 0;
}

bool vectorResize(Vector *v, size_t newCapacity) {
    assert(v);

    if (newCapacity <= v->capacity) {
        return false;
    }

    void *values = realloc(v->values, newCapacity * v->elemSize);

    if (!values) {
        return false;
    }

    v->values = values;
    v->capacity = newCapacity;

    return true;
}

bool vectorPush(Vector *v, void *value) {
    assert(v);
    assert(value);

    // The vector is not large enough to store another value
    // It needs to be increased
    if (v->size + 1 > v->capacity) {
        size_t nextCapacity = (v->capacity == 0) ? 1 : v->capacity;

        while (nextCapacity < v->size + 1) {
            nextCapacity *= 2;
        }

        if (!vectorResize(v, nextCapacity)) {
            return false;
        }
    }

    memcpy((char*)v->values + v->size * v->elemSize, value, v->elemSize);

    v->size += 1;

    return true;
}

void *vectorAt(Vector *v, size_t i) {
    assert(v);

    if (i >= v->size) {
        return NULL;
    }

    return v->values + v->elemSize * i;
}

size_t vectorCapacity(const Vector *v) {
    assert(v);

    return v->capacity;
}

size_t vectorSize(const Vector *v) {
    assert(v);

    return v->size;
}

void *vectorRawValues(const Vector *v) {
    assert(v);

    return v->values;
}