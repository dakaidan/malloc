cmake_minimum_required(VERSION 3.24)
project(malloc C)

set(CMAKE_C_STANDARD 23)

add_executable(main main.c)

add_library(malloc SHARED malloc.c)

target_link_libraries(main malloc)
