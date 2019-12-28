#include "file_buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) (((a) <= (b)) ? (a) : (b))

FileBuffer *fbCreate(FILE *fp, size_t inBufCapacity, size_t outBufCapacity) {
    if (!fp) {
        return NULL;
    }

    uint8_t *inBuf = malloc(sizeof(*inBuf) * inBufCapacity);
    uint8_t *outBuf = malloc(sizeof(*inBuf) * outBufCapacity);

    if (!inBuf || !outBuf) {
        goto ERROR;
    }

    FileBuffer *fb = malloc(sizeof(*fb));

    if (!fb) {
        goto ERROR;
    }

    fb->fp = fp;

    fb->inBuf = inBuf;
    fb->inBufCapacity = inBufCapacity;
    fb->inBufSize = 0;

    fb->outBuf = outBuf;
    fb->outBufCapacity = outBufCapacity;
    fb->outBufSize = 0;

    return fb;
ERROR:
    free(inBuf);
    free(outBuf);

    return NULL;
}

void fbDelete(FileBuffer *fb) {
    if (fb) {
        fclose(fb->fp);
        free(fb->inBuf);
        free(fb->outBuf);
        free(fb);
    }
}

size_t fbRead(FileBuffer *fb, size_t size, void *dst) {
    assert(fb);
    assert(dst);

    size_t copiedBytes = 0;

    while (copiedBytes < size) {
        size_t bytesToCopy = MIN(fb->inBufSize, size - copiedBytes);
        if (bytesToCopy == 0) {

        }

        memcpy((uint8_t*)dst + copiedBytes, fb->outBuf, bytesToCopy);
        copiedBytes += bytesToCopy;
        fb->inBufSize -= bytesToCopy;
    }

    return 0;
}

bool fbWrite(FileBuffer *fb, const void *data, size_t size) {
    assert(fb);
    assert(fb->outBufCapacity > 0);
    assert(data);

    size_t writtenBytes = 0;

    while (writtenBytes < size) {
        size_t remainingBytes = fb->outBufCapacity - fb->outBufSize;
        size_t bytesToCopy = MIN(remainingBytes, size - writtenBytes);

        if (bytesToCopy > 0) {
            memcpy(fb->outBuf + fb->outBufSize, (uint8_t*)data + writtenBytes, bytesToCopy);
            fb->outBufSize += bytesToCopy;
            writtenBytes += bytesToCopy;
        }

        if (!fbFlush(fb)) {
            return false;
        }
    }

    return true;
}

bool fbFlush(FileBuffer *fb) {
    assert(fb);

    size_t result = fwrite(fb->outBuf, sizeof(*fb->outBuf), fb->outBufSize, fb->fp);

    if (result == fb->outBufSize) {
        fb->outBufSize = 0;
        return true;
    }

    return false;
}