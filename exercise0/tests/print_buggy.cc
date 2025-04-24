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

TEST(PrintBuggy, TestEmptyString) {
    expected = ": 0/0=0\n";
    call_counter = 0;
    actual = "";

    print_buggy("");

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintBuggy, TestSameCharacters) {
    auto input = "aaaaaaaa";
    expected = result_for_string(input);
    call_counter = 0;
    actual = "";

    print_buggy(input);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintBuggy, TestDifferentCharacters) {
    auto input = "abcedfghijklm";
    expected = result_for_string(input);
    call_counter = 0;
    actual = "";

    print_buggy(input);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}
TEST(PrintBuggy, TestSpecialCharacters) {
    auto input = "!\"§$%&/()=?";
    expected = result_for_string(input);
    call_counter = 0;
    actual = "";

    print_buggy(input);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintBuggy, TestAlotofCharacters) {
    auto base = "abcedfghijklm"s;
    base.reserve(1 << 25);
    while (base.size() < 1 << 24)
        base += base;

    expected = result_for_string(base);
    call_counter = 0;
    actual = "";

    print_buggy(base.c_str());

    // EXPECT_EQ(expected, actual); no print here as we do not want about 64 MB of slightly different strings printed to stdout, again
    EXPECT_EQ(call_counter, expected.size());
}

