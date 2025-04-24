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

TEST(PrintVerySlowly, TestEmptyString) {
    expected = "";
    call_counter = 0;
    actual = "";

    print_very_slowly('A', 0);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintVerySlowly, TestSingleLine) {
    expected = result_for_input('B', 1);
    call_counter = 0;
    actual = "";

    print_very_slowly('B', 1);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintVerySlowly, TestTwoLinesLine) {
    expected = result_for_input('C', 2);
    call_counter = 0;
    actual = "";

    print_very_slowly('C', 2);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}
TEST(PrintVerySlowly, TestTwentyLines) {
    expected = result_for_input('D', 20);
    call_counter = 0;
    actual = "";

    print_very_slowly('D', 20);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

TEST(PrintVerySlowly, TestAlotofCharacters) {
    expected = result_for_input('S', 500);
    call_counter = 0;
    actual = "";

    // unoptimized, this is supposed to be very slow
    print_very_slowly('S', 500);

    EXPECT_EQ(expected, actual);
    EXPECT_EQ(call_counter, expected.size());
}

