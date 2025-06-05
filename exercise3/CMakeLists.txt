cmake_minimum_required(VERSION 3.18)

project(ARTificialIntelligence)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

add_library(ART_debug_build INTERFACE)
add_library(ART_rel_build INTERFACE)
target_include_directories(ART_debug_build INTERFACE include)
target_include_directories(ART_rel_build INTERFACE include)

# compile options
target_compile_options(ART_debug_build INTERFACE -Wall -Wextra -Werror -Wpedantic -pedantic -Wno-unused-function -g)
target_compile_options(ART_rel_build INTERFACE -Wall -Wextra -Werror -Wpedantic -pedantic -Wno-unused-function -g -O3 -march=native)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
    target_compile_options(ART_debug_build INTERFACE -msse -msse2)
    target_compile_options(ART_rel_build INTERFACE -mpopcnt -mlzcnt -msse -msse2)
endif()

# Debug: Enable address and undefinined behaviour sanitization
#target_compile_options(ART_debug_build INTERFACE -fsanitize=address -fsanitize=undefined)
#target_link_options(ART_debug_build INTERFACE -fsanitize=address -fsanitize=undefined)
#target_link_libraries(ART_debug_build INTERFACE ubsan)

add_executable(ARTificialIntelligence src/main.cpp)
add_executable(ARTificialIntelligenceOpt src/main.cpp)

target_link_libraries(ARTificialIntelligence PRIVATE ART_debug_build)
target_link_libraries(ARTificialIntelligenceOpt PRIVATE ART_rel_build)

add_subdirectory(tests)
add_subdirectory(bench)
