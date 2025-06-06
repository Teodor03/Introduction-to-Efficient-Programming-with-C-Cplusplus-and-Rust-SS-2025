include(FetchContent)

FetchContent_Declare(
  googletest EXCLUDE_FROM_ALL
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)
function(googletest_file name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} PRIVATE ART_debug_build GTest::gtest_main m)
  target_include_directories(${name} PRIVATE ../tests)
  gtest_discover_tests(${name})
endfunction()

googletest_file(convex_polygon_test convex_polygon_test.cpp)
googletest_file(point_in_polygon_test point_in_polygon_test.cpp)
googletest_file(material_point_shader_test material_point_shader_test.cpp)
googletest_file(grid_test grid_test.cpp)
googletest_file(rusterizer_test rusterizer_test.cpp)
