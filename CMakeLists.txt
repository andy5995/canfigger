cmake_minimum_required(VERSION 3.2)
project(canfigger )

set(LIB_VERSION "\"0.1.0-dev\"")

enable_testing()
add_compile_options(-Wall -Wextra)
add_executable(test_canfigger test_canfigger.c)

add_library(canfigger canfigger.c)

add_test(NAME test_canfigger COMMAND test_canfigger)
target_link_libraries(test_canfigger canfigger)
add_compile_definitions(LIB_VERSION=${LIB_VERSION})
