#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#include <zlib.h>

struct BloomFilter;

/**
 * \brief Computes the canonical form of a kmer
 * 
 * The canonical form is the first of this two following elements
 * in the lexicographic order : the kmer itself and its reverse complement.
 * 
 * The given kmer will be modified to store the canonical form
 * 
 * @param kmer a string containing the kmer, will be modified
 * @param len length of the kmer
 * @return a pointer to the canonical form
 */
char *canonicalForm(char *kmer, size_t len);

/**
 * \brief Finds neighbors of the given kmer that are in the bloom filter
 * 
 * The length of the kmer must be greater than 1 and contains only
 * the following letters : A, T, C, G (in upper case).
 * 
 * The parameter "neighbors" will receive 4 chars maximum that correspond
 * to the last letter of the following kmer.
 * 
 * @param bf a pointer to a Bloom Filter structure
 * @param kmer
 * @param len length of the kmer
 * @param neighbors array that will store neighbors, could store 4 elements max
 * @return number of neighbors found if the Bloom Filter or a negative value in case of an error
 */
int findNeighbors(struct BloomFilter *bf, const char *kmer, size_t len, char *neighbors);

/**
 * \brief Returns the error message associated to the given gzip file
 * 
 * If the error code for this file is Z_ERRNO then the result of
 * strerror (non gzip error) will be returned.
 * 
 * @param fp file that has an error code
 * @return message associated to this error
 */
const char *gzFileError(gzFile fp);

#endif // UTILS_H