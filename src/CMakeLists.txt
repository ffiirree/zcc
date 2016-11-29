cmake_minimum_required(VERSION 3.6)

project(zcc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../bin)

aux_source_directory(. SRC_LISTS)

add_executable(zcc ${SRC_LISTS})