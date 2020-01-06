# Fasta compressor

Fasta compressor is a tool to compress and decompress file in [fasta format](https://en.wikipedia.org/wiki/FASTA_format). It was written in C and tested with the unit test framework [Unity](http://www.throwtheswitch.org/unity). I used cmake to manage configuration and build processes.

A [De Bruijn graph](https://en.wikipedia.org/wiki/De_Bruijn_graph) is used to compress and decompress files. It is represented as a [Bloom filter](https://en.wikipedia.org/wiki/Bloom_filter) in order to use less space.

This project was inspired by a lab subject from the University of Rennes, France. Here is a link of the lab website : http://bioinformatique.irisa.fr/BIF2019/. It may be offline next year if their authors decide to shut it down.

Two files are given in the samples folder. I don't own any of those files. All credits are for the authors of the previous given website.

## Dependencies

This project requires Zlib 

## Setup

First, you need to clone the project one your computer

`git clone git@github.com:mdesmarais/FastaCompressor.git`

Then you need to download Unity which is a submodule of the project. This is done with the following commands :

```
git submodule init
git submodule update
```

Finally, you can configure the project with cmake and compile it :

```
cmake .
make
```

Two executables will be created in the src folder : **fasta_compressor** and **fasta_decompressor**.

## Usage

You can find two files in the samples folder for testing the tool. Assuming the compression tool is built in the src folder, you can use this command :  
`./src/fasta_compressor samples/ecoli_sample_500Kb_reads_30x.fasta`

Two files will be created :

* .graph.gz file that contains a serialized Bloom filter
* .comp file that contains the compressed reads from the origin file

They are both required for the decompression.

The decompression is done with :  
`./src/fasta_decompressor samples/ecoli_sample_500Kb_reads_30x.comp`

The tool can be configured with some parameters. To get a list of all available parameters, you must call one of the executable with the argument "-?" or "--help" : `./src/fasta_decompressor --help`

# Tests

All tests are built by default when you execute the cmake command, I did not provide an option to disable them yet.

To execute all tests, you can use the command `ctest` in the directory where the project was built.