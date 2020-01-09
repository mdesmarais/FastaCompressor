project(FastaCompressTest)

include_directories(${FastaCompressor_SOURCE_DIR})

LIST(APPEND test_files 
    test_bloom_filter.c test_de_bruijn_graph.c test_fasta.c 
    test_queue.c test_string_utils.c 
    test_utils.c test_vector.c)

foreach(test_file ${test_files})
    get_filename_component(test_name ${test_file} NAME_WE)
    message(${test_name})
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} libfasta unity ZLIB::ZLIB)

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()