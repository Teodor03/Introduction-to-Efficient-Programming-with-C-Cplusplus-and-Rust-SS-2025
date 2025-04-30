cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic -g)
include(GoogleTest)
function(add_googletest name)
  add_executable(${name} ../src/print.c ${ARGN})
  target_link_libraries(${name} GTest::gtest_main)
  gtest_discover_tests(${name})
endfunction()


add_googletest(print_hello_world print_hello_world.cc)
add_googletest(print_buggy print_buggy.cc)
add_googletest(print_leaky print_leaky.cc)
add_googletest(print_very_slowly print_very_slowly.cc)
# new test cases
add_googletest(buggy_bench_test buggy_bench_test.cc)
add_googletest(fast_bench_test fast_bench_test.cc)
add_googletest(slow_bench_test slow_bench_test.cc)

