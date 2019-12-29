project(FastaCompressor)

LIST(APPEND source_files 
    bloom_filter.c file_buffer.c
    murmur3.c string_utils.c utils.c vector.c)

add_library(libfasta STATIC ${source_files})
set_target_properties(libfasta PROPERTIES ARCHIVE_OUTPUT_NAME "${PREFIX}fasta${SUFFIX}")