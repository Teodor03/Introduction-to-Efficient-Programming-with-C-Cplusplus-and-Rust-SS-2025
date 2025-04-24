extern "C" {
#include "print.h"
}

#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <algorithm>
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
    std::sort(input.begin(), input.end());
    return input;
}

TEST(PrintLeaky, TestEmptyString) {
    expected = "";
    call_counter = 0;
    actual = "";

    print_leaky("");

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintLeaky, TestSameCharacters) {
    auto input = "aaaaaaaa";
    expected = input;
    call_counter = 0;
    actual = "";

    print_leaky(input);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintLeaky, TestDifferentCharacters) {
    auto input = "mlkjihgfedba";
    expected = result_for_string(input);
    call_counter = 0;
    actual = "";

    print_leaky(input);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}
TEST(PrintLeaky, TestSpecialCharacters) {
    auto input = "!\"§$%&/()=?";
    expected = result_for_string(input);
    call_counter = 0;
    actual = "";

    print_leaky(input);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintLeaky, TestAlotofCharacters) {
    auto base = "abcdefghijklm"s;
    base.reserve(1 << 15);
    while (base.size() < 1 << 14) // slight downgrade for ... reasons
        base += base;

    expected = result_for_string(base);
    call_counter = 0;
    actual = "";

    print_leaky(base.c_str());

    // EXPECT_EQ(expected, actual); no print here as we do not want about 64 MB of slightly different strings printed to stdout, again
    EXPECT_EQ(call_counter, expected.size());
}

