#include <benchmark/benchmark.h>

extern "C" {
#include "print.h"


// not interesting for benchmark
void effi_putchar(char) {}
}



static void slow_bench(benchmark::State &state) {
    for (auto _ : state) print_very_slowly('D', 15000);
}
BENCHMARK(slow_bench);
