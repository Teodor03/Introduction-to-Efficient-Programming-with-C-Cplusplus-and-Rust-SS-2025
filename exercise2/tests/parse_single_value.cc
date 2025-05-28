#include <gtest/gtest.h>
#include <string>
#include <string_view>

#include "jayson.hpp"
#include "jayson_fixed.hpp"

#include "jayson_testhelper.hpp"

using namespace std::literals;

//------------------------------------------------------------------------------
// BASIC PARSING TESTS
//------------------------------------------------------------------------------

TEST(ParseSingleValue, BasicTypes) {
    // Test each basic type parses correctly
    check_string_value("\"hello\"", "hello");
    check_integer_value("42", 42);
    check_float_value("3.14", 3.14);
    check_boolean_value("true", true);
    check_boolean_value("false", false);
    check_null_value("null");
}

TEST(ParseSingleValue, TypeConversion) {
    // Test that tokens are converted to correct jayson_element types

    // String tokens become jayson_string
    test_parse_both("\"test\"", [](auto, auto string_result) {
        ASSERT_TRUE(string_result);
        EXPECT_EQ(string_result->get_type(), jayson::jayson_types::STRING);
        EXPECT_TRUE(string_result->to_string());
        EXPECT_FALSE(string_result->to_integer());
        EXPECT_FALSE(string_result->to_float());
        EXPECT_FALSE(string_result->to_boolean());
        EXPECT_FALSE(string_result->to_none());
        EXPECT_FALSE(string_result->to_object());
        EXPECT_FALSE(string_result->to_array());
    });

    // Integer tokens become jayson_integer
    test_parse_both("123", [](auto, auto int_result) {
        ASSERT_TRUE(int_result);
        EXPECT_EQ(int_result->get_type(), jayson::jayson_types::INTEGER);
        EXPECT_TRUE(int_result->to_integer());
        EXPECT_FALSE(int_result->to_string());
        EXPECT_FALSE(int_result->to_float());
        EXPECT_FALSE(int_result->to_boolean());
        EXPECT_FALSE(int_result->to_none());
        EXPECT_FALSE(int_result->to_object());
        EXPECT_FALSE(int_result->to_array());
    });

    // Float tokens become jayson_float
    test_parse_both("123.45", [](auto, auto float_result) {
        ASSERT_TRUE(float_result);
        EXPECT_EQ(float_result->get_type(), jayson::jayson_types::FLOAT);
        EXPECT_TRUE(float_result->to_float());
        EXPECT_FALSE(float_result->to_string());
        EXPECT_FALSE(float_result->to_integer());
        EXPECT_FALSE(float_result->to_boolean());
        EXPECT_FALSE(float_result->to_none());
        EXPECT_FALSE(float_result->to_object());
        EXPECT_FALSE(float_result->to_array());
    });

    // Boolean tokens become jayson_boolean
    test_parse_both("true", [](auto, auto bool_result) {
        ASSERT_TRUE(bool_result);
        EXPECT_EQ(bool_result->get_type(), jayson::jayson_types::BOOLEAN);
        EXPECT_TRUE(bool_result->to_boolean());
        EXPECT_FALSE(bool_result->to_string());
        EXPECT_FALSE(bool_result->to_integer());
        EXPECT_FALSE(bool_result->to_float());
        EXPECT_FALSE(bool_result->to_none());
        EXPECT_FALSE(bool_result->to_object());
        EXPECT_FALSE(bool_result->to_array());
    });

    // Null tokens become jayson_none
    test_parse_both("null", [](auto, auto null_result) {
        ASSERT_TRUE(null_result);
        EXPECT_EQ(null_result->get_type(), jayson::jayson_types::NONE);
        EXPECT_TRUE(null_result->to_none());
        EXPECT_FALSE(null_result->to_string());
        EXPECT_FALSE(null_result->to_integer());
        EXPECT_FALSE(null_result->to_float());
        EXPECT_FALSE(null_result->to_boolean());
        EXPECT_FALSE(null_result->to_object());
        EXPECT_FALSE(null_result->to_array());
    });
}

TEST(ParseSingleValue, LeadingZeros) {
    // Leading zeros should be allowed (per user feedback)
    check_integer_value("01", 1);
    check_integer_value("007", 7);
    check_integer_value("0123", 123);
    check_float_value("01.23", 1.23);
    check_float_value("007.89", 7.89);
}

TEST(ParseSingleValue, NegativeNumbers) {
    check_integer_value("-42", -42);
    check_integer_value("-0", 0);
    check_float_value("-3.14", -3.14);
    check_float_value("-0.0", -0.0);
}

TEST(ParseSingleValue, ScientificNotation) {
    check_float_value("1e5", 1e5);
    check_float_value("1E5", 1e5);
    check_float_value("1.23e4", 1.23e4);
    check_float_value("1e-5", 1e-5);
    check_float_value("-1.23e-4", -1.23e-4);
}

//------------------------------------------------------------------------------
// PARSING ERROR TESTS
//------------------------------------------------------------------------------

TEST(ParseSingleValue, EmptyInput) {
    check_parse_fails("");
    check_parse_fails("   ");
    check_parse_fails("\t\n\r");
}

TEST(ParseSingleValue, InvalidTokenSequences) {
    // These should fail because they're not valid single values
    check_parse_fails(",");
    check_parse_fails(":");
    check_parse_fails("{");
    check_parse_fails("}");
    check_parse_fails("[");
    check_parse_fails("]");
}

TEST(ParseSingleValue, CommentsNotValues) {
    // Comments alone should not parse as values
    check_parse_fails("// this is a comment");
}

//------------------------------------------------------------------------------
// TYPE DISTINCTION TESTS
//------------------------------------------------------------------------------

TEST(ParseSingleValue, StringVsKeyword) {
    // Ensure string representations of keywords are strings, not keywords
    check_string_value("\"true\"", "true");
    check_string_value("\"false\"", "false");
    check_string_value("\"null\"", "null");

    // And that actual keywords are their proper types
    check_boolean_value("true", true);
    check_boolean_value("false", false);
    check_null_value("null");
}

TEST(ParseSingleValue, StringVsNumber) {
    // Ensure string representations of numbers are strings, not numbers
    check_string_value("\"42\"", "42");
    check_string_value("\"-3.14\"", "-3.14");
    check_string_value("\"1e5\"", "1e5");

    // And that actual numbers are their proper types
    check_integer_value("42", 42);
    check_float_value("-3.14", -3.14);
    check_float_value("1.0e5", 1e5);
    check_float_value("1e5", 1e5);
}

TEST(ParseSingleValue, IntegerVsFloat) {
    // Integer format should parse as integer
    check_integer_value("42", 42);
    check_integer_value("-123", -123);
    check_integer_value("0", 0);

    // Float format should parse as float
    check_float_value("42.0", 42.0);
    check_float_value("0.0", 0.0);
    check_float_value("123.456", 123.456);
    check_float_value("1e5", 1e5);
}

//------------------------------------------------------------------------------
// WHITESPACE AND COMMENTS HANDLING
//------------------------------------------------------------------------------

TEST(ParseSingleValue, WhitespaceIgnored) {
    // Whitespace around values should be ignored (tokenizer already handles this)
    check_integer_value("  42  ", 42);
    check_string_value("  \"hello\"  ", "hello");
    check_boolean_value("  true  ", true);
    check_null_value("  null  ");
}

TEST(ParseSingleValue, CommentsIgnored) {
    // Comments should be ignored by parser
    check_integer_value("42 // this is a comment", 42);
    check_string_value("\"hello\" // comment", "hello");
    check_boolean_value("true // comment", true);
    check_null_value("null // comment");
}
