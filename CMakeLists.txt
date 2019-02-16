cmake_minimum_required(VERSION 3.9)
project(libctx C)

set(CMAKE_C_STANDARD 99)

add_executable(libctx_example_sum_prod libctx_example_sum_prod.c)
add_executable(libctx_example_performance libctx_example_performance.c)

add_executable(libctx_example_sum_prod_x86 libctx_example_sum_prod.c)
set_target_properties(libctx_example_sum_prod_x86 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
add_executable(libctx_example_performance_x86 libctx_example_performance.c)
set_target_properties(libctx_example_performance_x86 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
