#include "string_utils.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int8_t atoi8(const char *input) {
    int8_t val;

    sscanf(input, "%" SCNd8, &val);

    return val;
}

int64_t atoi64(const char *input) {
    int64_t val;

    sscanf(input, "%" SCNd64, &val);

    return val;
}

size_t pathExtension(const char *path, size_t pathLength, const char *ext, size_t extLength, char *buffer, size_t bufferLength) {
    assert(path);
    assert(ext);
    assert(buffer);

    if (pathLength > bufferLength || pathLength == 0) {
        return 0;
    }

    memcpy(buffer, path, pathLength);

    if (extLength == 0 || path[pathLength - 1] == '/' || path[pathLength - 1] == '\\') {
        return pathLength;
    }

    bool hasExt = false;
    size_t pos = 0;

    for (size_t i = pathLength - 1;i > 0;i--) {
        if (path[i] == '/' || path[i] == '\\') {
            break;
        }

        if (path[i] == '.') {
            pos = i;
            hasExt = true;
            break;
        }
    }

    if (hasExt) {
        pos += 1;
    }
    else {
        pos = pathLength + 1;
    }

    if (pos + extLength >= bufferLength) {
        return 0;
    }

    memcpy(buffer + pos, ext, extLength);

    buffer[pos - 1] = '.';
    buffer[pos + extLength] = '\0';

    return pos + extLength;
}

void reverseComplement(char *input, size_t len) {
    reverseString(input, len);

    for (size_t i = 0;i < len;i++) {
        switch(input[i]) {
            case 'A':
                input[i] = 'T';
                break;
            case 'T':
                input[i] = 'A';
                break;
            case 'C':
                input[i] = 'G';
                break;
            case 'G':
                input[i] = 'C';
                break;
            default:
                break;
        }
    }
}

void reverseString(char *input, size_t len) {
    assert(input);

    if (len == 0) {
        return;
    }

    size_t i = 0;
    size_t j = len - 1;

    while (i < j) {
        char c = input[i];
        input[i] = input[j];
        input[j] = c;

        i++;
        j--;
    }
}