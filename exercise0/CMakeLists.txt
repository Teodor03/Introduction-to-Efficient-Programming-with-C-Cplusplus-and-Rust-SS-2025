cmake_minimum_required(VERSION 3.18)

project(HelloWorld)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(include)
include_directories(src)
include_directories(bench)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic -g)

add_executable(HelloWorld src/main.c src/print.c)

add_executable(HelloWorldOpt src/main.c src/print.c)
target_compile_options(HelloWorldOpt PRIVATE -O3 -march=native)


add_subdirectory(tests)
add_subdirectory(bench)

