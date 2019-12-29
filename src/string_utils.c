#include "string_utils.h"

#include <assert.h>
#include <string.h>

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