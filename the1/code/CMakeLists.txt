cmake_minimum_required(VERSION 3.8)
project(ceng334)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES server.c)
add_executable(ceng334 ${SOURCE_FILES})