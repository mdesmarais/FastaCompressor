project(FastaCompressor)

LIST(APPEND source_files file_buffer.c string_utils.c bloom_filter.c murmur3.c)

add_library(libfasta STATIC ${source_files})
set_target_properties(libfasta PROPERTIES ARCHIVE_OUTPUT_NAME "${PREFIX}fasta${SUFFIX}")