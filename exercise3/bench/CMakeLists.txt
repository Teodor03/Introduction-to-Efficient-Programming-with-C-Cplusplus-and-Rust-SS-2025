include(FetchContent)

FetchContent_Declare(
  googlebench EXCLUDE_FROM_ALL
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG afa23b7699c17f1e26c88cbf95257b20d78d6247
)
FetchContent_MakeAvailable(googlebench)

add_library(ART_benches OBJECT bench_setup.cc)
target_link_libraries(ART_benches PUBLIC ART_rel_build)

function(googlebench_file name)
	add_executable(${name} ${ARGN})
	target_link_libraries(${name} PRIVATE ART_benches benchmark::benchmark_main m)
endfunction()

googlebench_file(rasterize_bench rasterize_bench.cc)
googlebench_file(shader_bench shader_bench.cc)
googlebench_file(cutout_bench cutout_bench.cc)
