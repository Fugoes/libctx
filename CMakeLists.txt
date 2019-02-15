cmake_minimum_required(VERSION 3.9)
project(libctx C)

set(CMAKE_C_STANDARD 99)

add_executable(libctx_example_sum libctx_example_sum.c)
add_executable(libctx_example_performance libctx_example_performance.c)
