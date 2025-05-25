cmake_minimum_required(VERSION 3.18)

project(JaySon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include_directories(include)
include_directories(src)
include_directories(bench)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic -g -march=native)

add_executable(JaySonParser src/main.cc src/jayson.cc)
add_executable(JaySonParserOpt src/main.cc src/jayson.cc)
target_compile_options(JaySonParserOpt PRIVATE -O3 -march=native)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    target_compile_options(JaySonParserOpt PRIVATE -mpopcnt -mlzcnt -msse)
endif()


add_subdirectory(tests)
add_subdirectory(bench)
