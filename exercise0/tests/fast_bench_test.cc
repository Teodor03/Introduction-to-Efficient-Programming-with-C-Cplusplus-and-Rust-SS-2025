extern "C" {
#include "print.h"
}


#include <cstring>
#include <gtest/gtest.h>
#include <string>
using namespace std::literals;

static std::string expected{};
static std::string actual {};
static long long call_counter = 0;
extern "C" {
void effi_putchar(char c) {
    ASSERT_LT(call_counter, expected.size());
    EXPECT_EQ(expected.at(call_counter++), c);
    actual.push_back(c);
}
}

auto result_for_input(char c, size_t num) {
    auto result = ""s;
    for (size_t i = 0; i < num; ++i) {
        for (size_t j = 0; j < i; ++j)
            result += c;
        result += '\n';
    }
    return result;
}



/*
static void short_bench(benchmark::State &state) {
    for (auto _ : state) print_very_slowly('D', 20);
}
BENCHMARK(short_bench);
*/
TEST(FastBench, shortBench) {
    expected = result_for_input('D', 20);
    call_counter = 0;
    actual = "";

    // unoptimized, this is supposed to be very slow
    print_very_slowly('D', 20);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

/*
static void medium_bench(benchmark::State &state) {
    for (auto _ : state) print_very_slowly('D', 500);
}
BENCHMARK(medium_bench);
*/
TEST(FastBench, mediumBench) {
    expected = result_for_input('D', 500);
    call_counter = 0;
    actual = "";

    // unoptimized, this is supposed to be very slow
    print_very_slowly('D', 500);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}


