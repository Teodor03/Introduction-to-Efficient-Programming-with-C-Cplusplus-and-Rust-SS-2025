cmake_minimum_required(VERSION 3.18)
project(my_project)

set(CMAKE_CXX_STANDARD 20)
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

# Debug: Enable address and undefinined behaviour sanitization
#add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic -g -fsanitize=address -fsanitize=undefined)
#add_link_options(-fsanitize=address -fsanitize=undefined)
#target_link_libraries(${name} GTest::gtest_main m ubsan)

include(GoogleTest)
function(googletest_file name)
  add_executable(${name} ../src/jayson.cc ${ARGN})
  target_link_libraries(${name} GTest::gtest_main m)
  target_compile_options(${name} PRIVATE -Wall -Wextra -Werror -Wpedantic -pedantic -g)
  target_include_directories(${name} PRIVATE ../include ../tests)
  gtest_discover_tests(${name})
endfunction()

# Add test executables
googletest_file(tokenize_test tokenize_test.cc)
googletest_file(parse_single_value parse_single_value.cc)
googletest_file(parse_simple_structures_test parse_simple_structures_test.cc)
googletest_file(parse_complex_structures_test parse_complex_structures_test.cc)
googletest_file(parse_complete_documents_test parse_complete_documents_test.cc)
googletest_file(parse_error_test parse_error_test.cc)
googletest_file(tokenize_bench_test tokenize_bench_test.cc)
