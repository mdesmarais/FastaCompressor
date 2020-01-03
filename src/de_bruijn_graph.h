#ifndef DE_BRUIJN_GRAPH_H
#define DE_BRUIJN_GRAPH_H

#include <stdbool.h>
#include <stdio.h>

#include <zlib.h>

struct BloomFilter;

/**
 * \brief Creates a De Bruijn graph from a given fasta file
 * 
 * The file pointer must be an opened file in reading mode
 * and must be a fasta file.
 * 
 * If k is negative or greater than the length of a lecture, then the
 * function will return 0.
 * 
 * If an io error occured, then false will be returned.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param fp fasta file
 * @param k length of each kmer
 * @return true is the graph was correctly loaded, otherwise false
 */
bool createDBG(struct BloomFilter *bf, FILE *fp, int k);

/**
 * Inserts the canonical kmer form into the Bloom filter
 * 
 * If the kmer length k is negative or equals 0 then false will be returned.
 * The given kmer will be modified to store its canonical form.
 * 
 * @param bf a pointer to a Bloom filter structure, it will store the kmer
 * @param kmer kmer to store
 * @param k length of the kmer
 * @return true if the insertion succeed, false otherwise
 */
bool insertKmer(struct BloomFilter *bf, char *kmer, int k);

/**
 * \brief Loads a De Bruijn from a gzip file
 * 
 * The file must contain a serialized graph (See saveDBG for the format).
 * 
 * If an error occured during this decompression or 
 * during the reading (missing fields), then NULL will be returned.
 * 
 * This function returns an heap allocated Bloom filter if no error occured.
 * The user is in charge of the releasing the memory.
 * 
 * @param fp a file pointer to a gzip file
 * @return a pointer to a Bloom filter or NULL in case of an error
 */
struct BloomFilter *loadDBG(gzFile fp);

/**
 * \brief Writes a De Bruijn into a gzip file
 * 
 * The file must be opened in binary writing mode.
 * 
 * All fields are written with the order of the computer.
 * We assume that only the little endian order will be used.
 * 
 * The first 8 bytes represent the size (in bits) of the associated Bloom filter.
 * It is a signed integer number.
 * 
 * The next byte represents the number of hashs functions used to add a word into the filter.
 * It is a signed char.
 * 
 * Then the last n bits represent the content of the filter, where n is the size in bits of the filter.
 * 
 * This functions returns true is the graph was correctly written into the disk or false
 * if an error occured.
 * 
 * @param bf a pointer to a Bloom filter structure (represents the De Bruin graph)
 * @param fp a pointer to a gzip file
 * @return true if no error occured, otherwise false
 */
bool saveDBG(struct BloomFilter *bf, gzFile fp);

#endif // DE_BRUIJN_GRAPH_H