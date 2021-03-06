project(FastaCompressor)

LIST(APPEND source_files 
    bloom_filter.c de_bruijn_graph.c fasta.c
    log.c murmur3.c queue.c string_utils.c utils.c vector.c)

add_library(libfasta STATIC ${source_files})
set_target_properties(libfasta PROPERTIES ARCHIVE_OUTPUT_NAME "${PREFIX}fasta${SUFFIX}")

add_executable(fasta_compress compress.c)
target_link_libraries(fasta_compress libfasta ZLIB::ZLIB)

find_package(Threads REQUIRED)

add_executable(fasta_decompress decompress.c decompress_thread.c)
target_link_libraries(fasta_decompress libfasta ZLIB::ZLIB Threads::Threads)