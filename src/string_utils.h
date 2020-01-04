#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
#include <stdint.h>

/**
 * \brief Converts a string to a signed byte number
 * 
 * @param input string that contains the number
 * @return one byte nulber
 */
int8_t atoi8(const char *input);

/**
 * \brief Converts a string to a signed 8 bytes number
 * 
 * @param input string that contains the number
 * @return 8 bytes number
 */
int64_t atoi64(const char *input);

/**
 * \brief Replaces the file path extension with the given one
 * 
 * This function returns 0 if the path length is 0 or if its length
 * is greather than the buffer capacity.
 * 
 * If the given path does not end with an extension, then the new extension will be added.
 * If the given path ends with several extensions, then only the last one will be changed.
 * 
 * @param path path where the given extension should be added
 * @param pathLength length of the given path
 * @param ext extension to add (without the dot)
 * @param extLength length of the extension to add, must be positive
 * @param buffer to store the new path
 * @param bufferLength length of the buffer
 * @return size of the new path
 */
size_t pathExtension(const char *path, size_t pathLength, const char *ext, size_t extLength, char *buffer, size_t bufferLength);

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