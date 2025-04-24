#include <benchmark/benchmark.h>

extern "C" {
#include "../include/print.h"


// not interesting for benchmark
void effi_putchar(char) {}
}

#include <string>
using namespace std::literals;

static void short_buggy_bench(benchmark::State &state) {
    auto init = "foo"s;
    while (init.size() < (1 << 6)) init += init;
    for (auto _ : state) print_buggy(init.c_str());
}
BENCHMARK(short_buggy_bench);

static void medium_buggy_bench(benchmark::State &state) {
    auto init = "foo"s;
    while (init.size() < (1 << 12)) init += init;
    for (auto _ : state) print_buggy(init.c_str());
}
BENCHMARK(medium_buggy_bench);

static void long_buggy_bench(benchmark::State &state) {
    auto init = "foo"s;
    while (init.size() < (1 << 25)) init += init;
    for (auto _ : state) print_buggy(init.c_str());
}
BENCHMARK(long_buggy_bench);
