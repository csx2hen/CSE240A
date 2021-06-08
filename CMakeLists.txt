cmake_minimum_required(VERSION 3.19)
project(CSE240A C)

set(CMAKE_C_STANDARD 11)

aux_source_directory(src SRC_LIST)

add_executable(predictor ${SRC_LIST})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
