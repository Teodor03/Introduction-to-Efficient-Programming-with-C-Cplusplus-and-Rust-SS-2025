#include <benchmark/benchmark.h>

extern "C" {
#include "print.h"


// not interesting for benchmark
void effi_putchar(char) {}
}


static void short_bench(benchmark::State &state) {
    for (auto _ : state) print_very_slowly('D', 20);
}
BENCHMARK(short_bench);

static void medium_bench(benchmark::State &state) {
    for (auto _ : state) print_very_slowly('D', 500);
}
BENCHMARK(medium_bench);
