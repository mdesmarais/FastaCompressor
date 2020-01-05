#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct BloomFilter {
    char *data;
    long size;
    int8_t nbhashs;
} BloomFilter;

#define bfNbHashs(bf) ((bf)->nbhashs)

/**
 * \brief Gets the size (in bytes) of the filter
 */
#define bfSize(bf) ((bf)->size)

/**
 * \brief Gets the size (in bits) of the filter
 */
#define bfBitSize(bf) ((bf)->size * 8)

/**
 * \brief Creates a new Bloom filter
 * 
 * The filter size must be strictely positive.
 * It represents the number of bytes that the filter could store.
 * 
 * The number of hash functions must be strictely negative
 * and less than 256. It will be stored as one byte
 * in its serialized form.
 * 
 * This function returns NULL when the parameters are not
 * valid or a memory allocation error occured.
 * 
 * @param n size of the filter (in bytes)
 * @param k number of hash functions
 * @return a pointer to an allocated BloomFilter structure
 */
BloomFilter *bfCreate(long n, int8_t k);

/**
 * \brief Frees the allocated memory for the given Bloom filter structure
 * 
 * @param bf a pointer to a dynamically allocated Bloom filter structure
 */
void bfDelete(BloomFilter *bf);

/**
 * \brief Fills the filter with some values
 * 
 * The pointed array must have the same length as
 * the filter size
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param bytes pointer to the first byte of an array
 */
void bfFill(BloomFilter *bf, int8_t *bytes);

/**
 * \brief Gets the bit value at index i
 * 
 * Parameter i must be positive and less than bfBitSize.
 * If pError is not a NULL pointer then its value will
 * be changed to one.
 * 
 * By default pError value will be set to zero.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param i index of the bit
 * @param pError pointer to an error variable (could be NULL)
 * @return value of the bit at index i
 */
char bfGetBit(BloomFilter *bf, long long i, int *pError);

/**
 * \brief Changes the bit value at index i
 * 
 * Parameter i must be positive and less than bfBitSize.
 * False will be returned if i out of bounds.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param i index of the bit
 * @return true if the bit value has been correctly updated, otherwise false
 */
bool bfSetBit(BloomFilter *bf, long long i);

/**
 * \brief Insert a new value into the filter
 * 
 * This value will be hashed with n hash functions, each result
 * corresponds to one bit in the filter.
 * 
 * The size parameter must be strictely positive.
 * 
 * This function returns false if the size is negative or
 * if a bit could not been inserted into the filter.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param value value to add
 * @param valSize size of the value (in bytes)
 * @return true if the value was correctly added, otherwise false
 */
bool bfAdd(BloomFilter *bf, void *value, int valSize);

/**
 * \brief Checks if the filter contains the given value
 * 
 * The size must be strictely positive.
 * 
 * The value will be hashed with n functions : if one hash does not
 * correspond to a positive bit in the filter then false will
 * be returned.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param value value that could be in the filter
 * @param valSize size of the value (in bytes)
 * @return true if the filter contains the value, otherwise false
 */
bool bfContains(BloomFilter *bf, void *value, int valSize);

#endif // BLOOM_FILTER_H
