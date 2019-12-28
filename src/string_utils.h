#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

/**
 * \brief Computes the reverse complement of a given string
 * 
 * The string must be in upper case and contains only
 * the following letters : A, T, C, G.
 * The given string will me modified if its length is greater than zero.
 * 
 * @param input string for computing the reverse complement
 * @param len length of the string
 */
void reverseComplement(char *input, size_t len);

/**
 * \brief Computes the reverse of a given string
 * 
 * The given string will be modified if its length is greater than zero.
 * 
 * @param input a string of length len
 * @param len length of the string
 */
void reverseString(char *input, size_t len);

#endif // STRING_UTILS_H