project(FastaCompressorPrj)
cmake_minimum_required(VERSION 3.0)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(ZLIB REQUIRED)

if (NOT ZLIB_FOUND)
    message(FATAL_ERROR "ZLib is missing")
endif()

add_subdirectory(src)

add_subdirectory(Unity)

enable_testing()
add_subdirectory(test)