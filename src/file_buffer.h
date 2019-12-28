#ifndef FILE_BUFFER_H
#define FILE_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct FileBuffer {
    FILE *fp;

    uint8_t *inBuf;
    size_t inBufSize;
    size_t inBufCapacity;

    uint8_t *outBuf;
    size_t outBufSize;
    size_t outBufCapacity;
} FileBuffer;

FileBuffer *fbCreate(FILE *fp, size_t inBufCapacity, size_t outBufCapacity);
void fbDelete(FileBuffer *fb);

size_t fbRead(FileBuffer *fb, size_t size, void *dst);

bool fbWrite(FileBuffer *fb, const void *data, size_t size);
bool fbFlush(FileBuffer *fb);

#endif // FILE_BUFFER_H