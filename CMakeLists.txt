project(FastaCompressorPrj)
cmake_minimum_required(VERSION 3.0)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(src)

add_subdirectory(Unity)

enable_testing()
add_subdirectory(test)