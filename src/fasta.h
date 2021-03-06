#ifndef FASTA_H
#define FASTA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct BloomFilter;
struct Vector;

/**
 * \brief Compresses the sequences of the input file into the output file
 * 
 * The given Bloom filter must contain all kmers of length k of all reads
 * contained in the input file.
 * 
 * The input file must be opened in reading mode.
 * The output file must be opened in writing mode.
 * 
 * The length of each kmer k must be strictely positive and less or equal than
 * the length of all reads. False will be returned if it is not the case.
 * 
 * All headers of the input file will be skipped.
 * All reads of the input file must have the same length.
 * 
 * The output file will start with the length of all reads.
 * Each read will be represented with its first kmer of length k,
 * may be followed by zero or more branchings.
 * A space separates the first kmer from the branchings, even if the read
 * does not have branchings.
 * 
 * The user will have to close the two files.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param in file pointer to the input file
 * @param out file pointer to the output file
 * @param k length of a kmer
 * @return true if no error occured, otherwise false
 * */
bool compressFile(struct BloomFilter *bf, FILE *in, FILE *out, int k);

/**
 * \brief Computes branchings that are required to find the original
 * sequence with its first kmer of length k
 * 
 * The length of each kmer k must be strictely positive and less or equal to len.
 * 
 * When a kmer has several neighbors in the Bloom filter then a branching is required.
 * A branching is the last letter of the next kmer.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param branchings a pointer to a Vector structure
 * @param seq origin sequence
 * @param len length of the sequence
 * @param k length of each kmer
 * @return true if no error occured, otherwise false
 */
bool computeBranchings(struct BloomFilter *bf, struct Vector *branchings, char *seq, int len, int k);

/**
 * \brief Decompresses reads into the output file
 * 
 * The first line of the input file must be the length of each reads.
 * They must have the same length.
 * 
 * @param bf a pointer to a Bloom filter structure
 * @param in pointer to an input file
 * @param out pointer to an output file
 * @param k length of each kmer
 */
bool decompressFile(struct BloomFilter *bf, FILE *in, FILE *out, int k);
bool decompressRead(struct BloomFilter *bf, struct Vector *branchings, char *read, int readLength, const char *firstKmer, int k);

/**
 * \brief Extracts branchings from a compressed read
 * 
 * A compressed read is represented by its firt kmer, a space
 * and may be by some branchings.
 * This function looks for those branchings after the space and
 * adds them into the given vector.
 * 
 * If the compressed read is not valid or an error occured
 * then a negative value will be returned.
 * 
 * @param branchings a pointer to a vector structure
 * @param line compressed read
 * @return number of branchings found or a negative value in an error occured
 */
int extractBranchings(struct Vector *branchings, const char *line);

#endif // FASTA_H