extern "C" {
#include "print.h"
}

#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <numeric>
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


auto result_for_string(std::string input) {
    auto sum = std::accumulate(input.begin(), input.end(), 0LL);
    char avrg = sum / input.size();
    input += ": " + std::to_string(sum) + '/' + std::to_string(input.size()) + '=' + avrg + '\n';
    return input;
}

/*
static void short_buggy_bench(benchmark::State &state) {
    auto init = "foo"s;
    while (init.size() < (1 << 6)) init += init;
    for (auto _ : state) print_buggy(init.c_str());
}
BENCHMARK(short_buggy_bench);
*/
TEST(BuggyBench, ShortBench) {
    auto init = "foo"s;
    while (init.size() < (1 << 6)) init += init;
    expected = result_for_string(init);
    call_counter = 0;
    actual = "";

    print_buggy(init.c_str());

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

/*
static void medium_buggy_bench(benchmark::State &state) {
    auto init = "foo"s;
    while (init.size() < (1 << 12)) init += init;
    for (auto _ : state) print_buggy(init.c_str());
}
BENCHMARK(medium_buggy_bench);
*/
TEST(BuggyBench, MediumBench) {
    auto init = "foo"s;
    while (init.size() < (1 << 12)) init += init;
    expected = result_for_string(init);
    call_counter = 0;
    actual = "";

    print_buggy(init.c_str());

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

/*
static void long_buggy_bench(benchmark::State &state) {
    auto init = "foo"s;
    while (init.size() < (1 << 23)) init += init;
    for (auto _ : state) print_buggy(init.c_str());
}
BENCHMARK(long_buggy_bench);
*/
TEST(BuggyBench, LongBench) {
    auto init = "foo"s;
    while (init.size() < (1 << 23)) init += init;
    expected = result_for_string(init);
    call_counter = 0;
    actual = "";

    print_buggy(init.c_str());

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}
