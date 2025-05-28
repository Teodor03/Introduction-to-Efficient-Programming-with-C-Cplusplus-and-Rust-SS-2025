#include <gtest/gtest.h>
#include <string>
#include <string_view>

#include "jayson.hpp"
#include "jayson_fixed.hpp"
#include "jayson_print.hpp"

#include "jayson_testhelper.hpp"

using namespace std::literals;

//------------------------------------------------------------------------------
// MALFORMED STRUCTURES
//------------------------------------------------------------------------------

TEST(ParseErrorHandling, UnmatchedDelimiters) {
    // Mismatched object/array delimiters
    check_parse_fails("{]");
    check_parse_fails("[}");
    check_parse_fails("({)");
    check_parse_fails("([)]");

    // Missing opening delimiters
    check_parse_fails("}");
    check_parse_fails("]");
    check_parse_fails("\"key\": \"value\"}");
    check_parse_fails("1, 2, 3]");

    // Missing closing delimiters
    check_parse_fails("{");
    check_parse_fails("[");
    check_parse_fails("{\"key\": \"value\"");
    check_parse_fails("[1, 2, 3");

    // Nested mismatches
    check_parse_fails("{\"array\": [1, 2}");
    check_parse_fails("{\"object\": {\"key\": \"value\"]}");
    check_parse_fails("[{\"key\": \"value\"]");
    check_parse_fails("[1, {2, 3}]");
}

TEST(ParseErrorHandling, IncompleteStructures) {
    // Incomplete objects
    check_parse_fails("{\"key\"");
    check_parse_fails("{\"key\":");
    check_parse_fails("{\"key\": ");
    check_parse_fails("{\"key\": \"value\",");
    check_parse_fails("{\"key\": \"value\", ");
    check_parse_fails("{\"key\": \"value\", \"key2\"");
    check_parse_fails("{\"key\": \"value\", \"key2\":");

    // Incomplete arrays
    check_parse_fails("[1");
    check_parse_fails("[1,");
    check_parse_fails("[1, ");
    check_parse_fails("[1, 2,");
    check_parse_fails("[1, 2, ");

    // Incomplete nested structures
    check_parse_fails("{\"nested\": {");
    check_parse_fails("{\"nested\": {\"key\"");
    check_parse_fails("{\"array\": [");
    check_parse_fails("{\"array\": [1");
    check_parse_fails("[{");
    check_parse_fails("[{\"key\"");
}

TEST(ParseErrorHandling, InvalidObjectSyntax) {
    // Missing colons in objects
    check_parse_fails("{\"key\" \"value\"}");
    check_parse_fails("{\"key1\": \"value1\", \"key2\" \"value2\"}");
    check_parse_fails("{\"key\" 42}");

    // Missing commas between key-value pairs
    check_parse_fails("{\"key1\": \"value1\" \"key2\": \"value2\"}");
    check_parse_fails("{\"a\": 1 \"b\": 2}");
    check_parse_fails("{\"x\": true \"y\": false}");

    // Invalid keys (non-strings)
    check_parse_fails("{42: \"value\"}");
    check_parse_fails("{true: \"value\"}");
    check_parse_fails("{null: \"value\"}");
    check_parse_fails("{[]: \"value\"}");
    check_parse_fails("{{}: \"value\"}");

    // Empty keys or values
    check_parse_fails("{: \"value\"}");
    check_parse_fails("{\"\": }");
    check_parse_fails("{\"key\": }");

    // Trailing commas
    check_parse_fails("{\"key\": \"value\",}");
    check_parse_fails("{\"a\": 1, \"b\": 2,}");
    check_parse_fails("{\"nested\": {\"inner\": \"value\",}}");

    // Wrong separators (comma instead of colon)
    check_parse_fails("{\"key\", \"value\"}");
}

TEST(ParseErrorHandling, InvalidArraySyntax) {
    // Missing commas between elements
    check_parse_fails("[1 2]");
    check_parse_fails("[\"a\" \"b\"]");
    check_parse_fails("[true false]");
    check_parse_fails("[1 2 3 4]");

    // Empty elements
    check_parse_fails("[1, , 3]");
    check_parse_fails("[, 2, 3]");
    check_parse_fails("[1, 2, ]");
    check_parse_fails("[,]");

    // Trailing commas
    check_parse_fails("[1,]");
    check_parse_fails("[1, 2, 3,]");
    check_parse_fails("[\"a\", \"b\",]");
    check_parse_fails("[[1, 2], [3, 4],]");
}

//------------------------------------------------------------------------------
// MALFORMED VALUES
//------------------------------------------------------------------------------

TEST(ParseErrorHandling, InvalidStrings) {
    // Unclosed strings
    check_parse_fails("\"");
    check_parse_fails("\"hello");
    check_parse_fails("\"hello world");
    check_parse_fails("\"unterminated string in object\": {");

    // Unquoted strings (as standalone values)
    check_parse_fails("hello");
    check_parse_fails("unquoted_string");
    check_parse_fails("{key: \"value\"}"); // Unquoted key
}

TEST(ParseErrorHandling, InvalidNumbers) {
    // Just signs or decimal points (should not parse as valid numbers)
    check_parse_fails("-");
    check_parse_fails("+");
    check_parse_fails(".");
    check_parse_fails("-.");
    check_parse_fails("+.");

    // Incomplete negative numbers with space
    check_parse_fails("- 42"); // Space after minus
    check_parse_fails("-  123");
}

TEST(ParseErrorHandling, InvalidKeywords) {
    // Incomplete boolean keywords
    check_parse_fails("tru");
    check_parse_fails("tr");
    check_parse_fails("t");
    check_parse_fails("fals");
    check_parse_fails("fal");
    check_parse_fails("fa");
    check_parse_fails("f");

    // Incomplete null keyword
    check_parse_fails("nul");
    check_parse_fails("nu");
    check_parse_fails("n");

    // Case variations (JSON is case-sensitive)
    check_parse_fails("True");
    check_parse_fails("TRUE");
    check_parse_fails("False");
    check_parse_fails("FALSE");
    check_parse_fails("Null");
    check_parse_fails("NULL");
    check_parse_fails("None");
    check_parse_fails("NONE");
    check_parse_fails("none");
}

//------------------------------------------------------------------------------
// INVALID TOKEN SEQUENCES
//------------------------------------------------------------------------------

TEST(ParseErrorHandling, UnexpectedTokens) {
    // Structural tokens in wrong places
    check_parse_fails(",");
    check_parse_fails(":");
    check_parse_fails(",42");
    check_parse_fails(":\"value\"");
    check_parse_fails("42,");
    check_parse_fails("\"string\":");

    // Invalid combinations
    check_parse_fails("42:43");
    check_parse_fails("\"key\"\"value\"");
    check_parse_fails("true:false");
    check_parse_fails("null,42");
}

TEST(ParseErrorHandling, InvalidNestedTokenSequences) {
    // Invalid tokens in object context
    check_parse_fails("{42}");
    check_parse_fails("{true}");
    check_parse_fails("{null}");
    check_parse_fails("{\"key\" = \"value\"}");

    // Invalid tokens in array context
    check_parse_fails("[\"key\": \"value\"]");
    check_parse_fails("[42: 43]");
    check_parse_fails("[=]");

    // Wrong delimiters for context
    check_parse_fails("{1, 2, 3}");
    check_parse_fails("[\"key\": \"value\"]");
}

//------------------------------------------------------------------------------
// PARSER STATE EDGE CASES
//------------------------------------------------------------------------------

TEST(ParseErrorHandling, EmptyInput) {
    // Completely empty
    check_parse_fails("");

    // Only whitespace
    check_parse_fails(" ");
    check_parse_fails("\t");
    check_parse_fails("\n");
    check_parse_fails("\r");
    check_parse_fails("   \t\n\r   ");

    // Only comments
    check_parse_fails("// comment");
    check_parse_fails("// comment 1\n// comment 2");
    check_parse_fails("// comment\n\n// another comment");
}

TEST(ParseErrorHandling, UnexpectedEndOfInput) {
    // Parser expecting more tokens
    check_parse_fails("{\"key\":");
    check_parse_fails("[1,");
    check_parse_fails("{\"nested\": {\"key\":");
    check_parse_fails("[{\"key\":");

    // Incomplete escape sequences
    check_parse_fails("\"\\");
    check_parse_fails("\"text\\");

    // Incomplete comments
    check_parse_fails("/");
}

TEST(ParseErrorHandling, InvalidCharacters) {
    // Control characters that shouldn't appear
    check_parse_fails("\x01");
    check_parse_fails("\x02");
    check_parse_fails("\x1F");

    // Random symbols
    check_parse_fails("@");
    check_parse_fails("#");
    check_parse_fails("$");
    check_parse_fails("%");
    check_parse_fails("&");
    check_parse_fails("*");
    check_parse_fails("=");
    check_parse_fails("?");
    check_parse_fails("|");
    check_parse_fails("~");
}

//------------------------------------------------------------------------------
// BOUNDARY CONDITIONS
//------------------------------------------------------------------------------

TEST(ParseErrorHandling, ExtremeNesting) {
    // Test parser limits with very deep nesting
    std::string deep_object;
    std::string deep_array;

    // Create extremely deep nesting (more than reasonable)
    int extreme_depth = 1000;

    // Deep object nesting
    for (int i = 0; i < extreme_depth; ++i) {
        deep_object += "{\"level" + std::to_string(i) + "\":";
    }
    deep_object += "42";
    for (int i = 0; i < extreme_depth; ++i) { deep_object += "}"; }

    // Deep array nesting
    for (int i = 0; i < extreme_depth; ++i) { deep_array += "["; }
    deep_array += "42";
    for (int i = 0; i < extreme_depth; ++i) { deep_array += "]"; }

    // These might fail due to stack overflow or implementation limits
    // if that happens, send us a message
    test_parse_both(deep_object,
        [](auto, auto result) { EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT); });
    test_parse_both(deep_array,
        [](auto, auto result) { EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY); });
}

TEST(ParseErrorHandling, VeryLongStrings) {
    // Test with extremely long strings
    std::string very_long_content(100000, 'a'); // 100KB string
    std::string very_long_string = "\"" + very_long_content + "\"";

    // If it fails due to limits, send us a message
    test_parse_both(very_long_string, [](auto, auto result) {
        EXPECT_EQ(result->get_type(), jayson::jayson_types::STRING);
        EXPECT_EQ(result->to_string()->get_string().length(), 100000);
    });
}

//------------------------------------------------------------------------------
// RECOVERY AND ROBUSTNESS
//------------------------------------------------------------------------------

TEST(ParseErrorHandling, MixedValidInvalid) {
    // Objects with some valid and some invalid pairs
    check_parse_fails("{\"valid\": true, invalid: false}");
    check_parse_fails("{\"a\": 1, \"b\" 2}");
    check_parse_fails("{\"x\": true, \"y\": }");

    // Arrays with some valid and some invalid elements
    check_parse_fails("[1, 2, , 4]");
    check_parse_fails("[\"valid\", invalid]");
    check_parse_fails("[true, false,]");
}

TEST(ParseErrorHandling, MemoryAndPerformance) {
    // Test that parser doesn't crash with pathological input

    // Very wide objects (many keys) with invalid syntax
    std::string wide_invalid = "{";
    for (int i = 0; i < 1000; ++i) { // Reduced from 10000 for performance
        if (i > 0) wide_invalid += ",";
        wide_invalid += "\"key" + std::to_string(i) + "\" invalid_value";
    }
    wide_invalid += "}";
    check_parse_fails(wide_invalid);

    // Very long arrays with invalid syntax
    std::string long_invalid = "[";
    for (int i = 0; i < 1000; ++i) {    // Reduced from 10000 for performance
        if (i > 0) long_invalid += " "; // Missing commas
        long_invalid += std::to_string(i);
    }
    long_invalid += "]";
    check_parse_fails(long_invalid);
}

//------------------------------------------------------------------------------
// GREEDY PARSER SPECIFIC TESTS
//------------------------------------------------------------------------------

TEST(ParseErrorHandling, GreedyParserValidCases) {
    // These should parse successfully (the valid part) with a greedy parser
    // We test that they DON'T fail

    check_integer_value("42 garbage", 42);
    check_boolean_value("true extra", true);
    check_float_value("1.5 .more.stuff", 1.5);

    test_parse_both("{\"valid\": true} garbage", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);
    });
}
