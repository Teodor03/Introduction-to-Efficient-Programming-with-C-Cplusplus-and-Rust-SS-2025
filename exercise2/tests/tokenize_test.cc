#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <cmath>

#include "jayson.hpp"
#include "jayson_fixed.hpp"
#include "jayson_print.hpp"

using namespace std::literals;

// Helper function to get all tokens from a string
std::vector<jayson::token> tokenize_all(std::string_view input) {
    auto tokenizer = jayson::tokenize(input);
    std::vector<jayson::token> tokens;
    
    while (auto token = tokenizer.get_next_token()) {
        tokens.push_back(token.value());
    }
    
    return tokens;
}

// Helper to check token type
void check_token_type(const jayson::token& token, jayson::token_type expected_type) {
    EXPECT_EQ(token.get_type(), expected_type);
}

// Helper to check string token
void check_string_token(const jayson::token& token, const std::string& expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::STRING);
    auto value = token.get_string();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}

// Helper to check integer token
void check_integer_token(const jayson::token& token, jayson::integer_type expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::INTEGER);
    auto value = token.get_integer();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}

// Helper to check float token
void check_float_token(const jayson::token& token, jayson::float_type expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::FLOAT);
    auto value = token.get_float();
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), expected_value);
}

// Helper to check boolean token
void check_boolean_token(const jayson::token& token, bool expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::BOOLEAN);
    auto value = token.get_boolean();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}

// Helper to check comment token
void check_comment_token(const jayson::token& token, const std::string& expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::COMMENT);
    auto value = token.get_comment();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}

// Test empty input
TEST(TokenizerTest, EmptyInput) {
    auto tokens = tokenize_all(""sv);
    EXPECT_TRUE(tokens.empty());
}

// Test whitespace handling
TEST(TokenizerTest, WhitespaceHandling) {
    auto tokens = tokenize_all("   \t\n\r   ");
    EXPECT_TRUE(tokens.empty());
}

// Test basic structural tokens
TEST(TokenizerTest, StructuralTokens) {
    auto tokens = tokenize_all("{}[]:,");
    ASSERT_EQ(tokens.size(), 6);
    
    check_token_type(tokens[0], jayson::token_type::OBJECT_BEGIN);
    check_token_type(tokens[1], jayson::token_type::OBJECT_END);
    check_token_type(tokens[2], jayson::token_type::ARRAY_BEGIN);
    check_token_type(tokens[3], jayson::token_type::ARRAY_END);
    check_token_type(tokens[4], jayson::token_type::COLON);
    check_token_type(tokens[5], jayson::token_type::COMMA);
}

// Test string tokens
TEST(TokenizerTest, StringTokens) {
    auto tokens = tokenize_all(R"("" "hello" "special chars: //" "\")");
    ASSERT_EQ(tokens.size(), 4);
    
    check_string_token(tokens[0], "");
    check_string_token(tokens[1], "hello");
    check_string_token(tokens[2], "special chars: //");
    check_string_token(tokens[3], "\\");
}

// Test integer tokens
TEST(TokenizerTest, IntegerTokens) {
    auto tokens = tokenize_all("0 42 -123 9223372036854775807");
    ASSERT_EQ(tokens.size(), 4);
    
    check_integer_token(tokens[0], 0);
    check_integer_token(tokens[1], 42);
    check_integer_token(tokens[2], -123);
    check_integer_token(tokens[3], 9223372036854775807);
}

// Test float tokens
TEST(TokenizerTest, FloatTokens) {
    auto tokens = tokenize_all("0.0 3.14 -2.718 1.234e5 6.022E23");
    // jayson::dump_tokens(tokens);
    ASSERT_EQ(tokens.size(), 5);
    
    check_float_token(tokens[0], 0.0);
    check_float_token(tokens[1], 3.14);
    check_float_token(tokens[2], -2.718);
    check_float_token(tokens[3], 1.234e5);
    check_float_token(tokens[4], 6.022e23);
}

// Test boolean tokens
TEST(TokenizerTest, BooleanTokens) {
    auto tokens = tokenize_all("true false");
    ASSERT_EQ(tokens.size(), 2);
    
    check_boolean_token(tokens[0], true);
    check_boolean_token(tokens[1], false);
}

// Test null token
TEST(TokenizerTest, NullToken) {
    auto tokens = tokenize_all("null");
    ASSERT_EQ(tokens.size(), 1);
    
    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::NONE);
}

// Test comment tokens
TEST(TokenizerTest, CommentTokens) {
    auto tokens = tokenize_all("// This is a comment\n// Another comment");
    ASSERT_EQ(tokens.size(), 2);
    
    check_comment_token(tokens[0], " This is a comment");
    check_comment_token(tokens[1], " Another comment");
}

// Test mixed tokens
TEST(TokenizerTest, MixedTokens) {
    auto tokens = tokenize_all(
        "{\n"
        "  \"name\": \"John\",\n"
        "  \"age\": 30,\n"
        "  \"is_student\": false,\n"
        "  \"grades\": [95.5, 87.0, 91.2],\n"
        "  // This is a comment\n"
        "  \"address\": null\n"
        "}"
    );
    
    // Enable for better debug output
    // jayson::dump_tokens(tokens);
    ASSERT_EQ(tokens.size(), 28);
    
    // First few tokens
    check_token_type(tokens[0], jayson::token_type::OBJECT_BEGIN);
    check_string_token(tokens[1], "name");
    check_token_type(tokens[2], jayson::token_type::COLON);
    check_string_token(tokens[3], "John");
    check_token_type(tokens[4], jayson::token_type::COMMA);
    
    // Comment token
    check_comment_token(tokens[23], " This is a comment");
}

// Test peek_next_token functionality
TEST(TokenizerTest, PeekNextToken) {
    auto tokenizer = jayson::tokenize("{\"key\": 42}");
    
    // Peek first token
    auto peeked1 = tokenizer.peek_next_token();
    ASSERT_TRUE(peeked1.has_value());
    EXPECT_EQ(peeked1.value().get_type(), jayson::token_type::OBJECT_BEGIN);
    
    // Get first token
    auto token1 = tokenizer.get_next_token();
    ASSERT_TRUE(token1.has_value());
    EXPECT_EQ(token1.value().get_type(), jayson::token_type::OBJECT_BEGIN);
    
    // Peek and get second token
    auto peeked2 = tokenizer.peek_next_token();
    ASSERT_TRUE(peeked2.has_value());
    EXPECT_EQ(peeked2.value().get_type(), jayson::token_type::STRING);
    
    auto token2 = tokenizer.get_next_token();
    ASSERT_TRUE(token2.has_value());
    EXPECT_EQ(token2.value().get_type(), jayson::token_type::STRING);
    
    // Continue until end
    (void)tokenizer.get_next_token(); // :
    (void)tokenizer.get_next_token(); // 42
    (void)tokenizer.get_next_token(); // }
    
    // No more tokens
    auto peeked_end = tokenizer.peek_next_token();
    EXPECT_FALSE(peeked_end.has_value());
    
    auto token_end = tokenizer.get_next_token();
    EXPECT_FALSE(token_end.has_value());
}

// Test number edge cases
TEST(TokenizerTest, NumberEdgeCases) {
    // Test extreme values
    auto tokens = tokenize_all("0 -0");

    // Enable for better debug output
    // jayson::dump_tokens(tokens);
    
    ASSERT_EQ(tokens.size(), 2);

    check_integer_token(tokens[0], 0);
    check_integer_token(tokens[1], 0);
}

// Test complex JSON
TEST(TokenizerTest, ComplexJSON) {
    std::string json = R"({
  "glossary": {
    "title": "example glossary",
    "GlossDiv": {
      "title": "S",
      "GlossList": {
        // This is a comment
        "GlossEntry": {
          "ID": "SGML",
          "SortAs": "SGML",
          "GlossTerm": "Standard Generalized Markup Language",
          "Acronym": "SGML",
          "Abbrev": "ISO 8879:1986",
          "GlossDef": {
            "para": "A meta-markup language, used to create markup languages such as DocBook.",
            "GlossSeeAlso": ["GML", "XML"]
          },
          "GlossSee": "markup"
        }
      }
    }
  }
})";

    auto tokens = tokenize_all(json);
    EXPECT_GT(tokens.size(), 30);
    
    // Check some specific tokens
    check_token_type(tokens[0], jayson::token_type::OBJECT_BEGIN);
    check_string_token(tokens[1], "glossary");
    
    // Find the comment
    bool found_comment = false;
    for (const auto& token : tokens) {
        if (token.get_type() == jayson::token_type::COMMENT) {
            found_comment = true;
            check_comment_token(token, " This is a comment");
            break;
        }
    }
    EXPECT_TRUE(found_comment);
}

// Test tokenizing incomplete input
TEST(TokenizerTest, IncompleteInput) {
    // Incomplete string
    auto tokenizer1 = jayson::tokenize("\"incomplete string");
    auto token1 = tokenizer1.get_next_token();
    EXPECT_FALSE(token1.has_value());
    
    // Incomplete true
    auto tokenizer2 = jayson::tokenize("tru");
    auto token2 = tokenizer2.get_next_token();
    EXPECT_FALSE(token2.has_value());
    
    // Incomplete null
    auto tokenizer3 = jayson::tokenize("nul");
    auto token3 = tokenizer3.get_next_token();
    EXPECT_FALSE(token3.has_value());
}

// Test tokenizing invalid input
TEST(TokenizerTest, InvalidInput) {
    // Invalid character
    auto tokenizer1 = jayson::tokenize("@");
    auto token1 = tokenizer1.get_next_token();
    EXPECT_FALSE(token1.has_value());
    
    // Invalid number format
    auto tokenizer2 = jayson::tokenize("e12");
    auto token2 = tokenizer2.get_next_token();
    EXPECT_FALSE(token2.has_value());
    
    // Invalid comment (missing second slash)
    auto tokenizer3 = jayson::tokenize("/comment");
    auto token3 = tokenizer3.get_next_token();
    EXPECT_FALSE(token3.has_value());

    // Valid token followed by invalid character
    auto tokenizer4 = jayson::tokenize("12 @");
    auto token4 = tokenizer4.get_next_token();
    EXPECT_TRUE(token4.has_value());
    auto token5 = tokenizer4.get_next_token();
    EXPECT_FALSE(token5.has_value());
}

// Test the token original content
TEST(TokenizerTest, TokenOriginal) {
    auto tokens = tokenize_all("{\"test\": 123}");
    ASSERT_EQ(tokens.size(), 5);
    
    EXPECT_EQ(tokens[0].get_original(), "{");
    EXPECT_EQ(tokens[1].get_original(), "\"test\"");
    EXPECT_EQ(tokens[2].get_original(), ":");
    EXPECT_EQ(tokens[3].get_original(), "123");
    EXPECT_EQ(tokens[4].get_original(), "}");
}

// Test whitespace between every possible token type
TEST(TokenizerTest, WhitespaceComprehensive) {
    // Every whitespace character between each token type
    auto tokens = tokenize_all(
        "{ \n"
        " \t \"key1\" \r : \t\n\r 42 \t , \n"
        " \r \"key2\" \t : \r\n [ \r"
        " \t true \n , \r"
        " \t false \t , \n"
        " \r null \t , \r\n"
        " \t 3.14 \n"
        " \r ] \t"
        " \n }"
    );
    
    ASSERT_EQ(tokens.size(), 17);
    
    check_token_type(tokens[0], jayson::token_type::OBJECT_BEGIN);
    check_token_type(tokens[1], jayson::token_type::STRING); // "key1"
    check_token_type(tokens[2], jayson::token_type::COLON);
    check_token_type(tokens[3], jayson::token_type::INTEGER); // 42
    check_token_type(tokens[4], jayson::token_type::COMMA);
    check_token_type(tokens[5], jayson::token_type::STRING); // "key2"
    check_token_type(tokens[6], jayson::token_type::COLON);
    check_token_type(tokens[7], jayson::token_type::ARRAY_BEGIN);
    check_token_type(tokens[8], jayson::token_type::BOOLEAN); // true
    check_token_type(tokens[9], jayson::token_type::COMMA);
    check_token_type(tokens[10], jayson::token_type::BOOLEAN); // false
    check_token_type(tokens[11], jayson::token_type::COMMA);
    check_token_type(tokens[12], jayson::token_type::NONE); // null
    check_token_type(tokens[13], jayson::token_type::COMMA);
    check_token_type(tokens[14], jayson::token_type::FLOAT); // 3.14
    check_token_type(tokens[15], jayson::token_type::ARRAY_END);
    check_token_type(tokens[16], jayson::token_type::OBJECT_END);
}

// Test excessive whitespace
TEST(TokenizerTest, ExcessiveWhitespace) {
    auto tokens = tokenize_all(
        "\n\n\n\n"
        "      {        "
        "\t\t\t\t\t"
        "            \"key\"          "
        "\r\r\r\r"
        "       :         "
        "\n\t\r \n\t\r "
        "             42             "
        "\n\n\n\n"
        "       }        "
        "\t\t\t\t"
    );
    
    ASSERT_EQ(tokens.size(), 5);
    check_token_type(tokens[0], jayson::token_type::OBJECT_BEGIN);
    check_token_type(tokens[1], jayson::token_type::STRING);
    check_token_type(tokens[2], jayson::token_type::COLON);
    check_token_type(tokens[3], jayson::token_type::INTEGER);
    check_token_type(tokens[4], jayson::token_type::OBJECT_END);
}

// Test whitespace in strings and comments
TEST(TokenizerTest, WhitespaceInStringsAndComments) {
    auto tokens = tokenize_all(
        "{\n"
        "  \"key with spaces\": \"value with \\t tab and \\n newline\",\n"
        "  // Comment with    lots   of    spaces    \n"
        "  \"another\": 42\n"
        "}"
    );
    
    ASSERT_EQ(tokens.size(), 10);
    
    // Check that whitespace is preserved correctly in strings
    auto string_value = tokens[1].get_string();
    ASSERT_TRUE(string_value.has_value());
    EXPECT_EQ(string_value.value(), "key with spaces");
    
    string_value = tokens[3].get_string();
    ASSERT_TRUE(string_value.has_value());
    EXPECT_EQ(string_value.value(), "value with \\t tab and \\n newline");
    
    // Check that whitespace is preserved in comments
    auto comment_value = tokens[5].get_comment();
    ASSERT_TRUE(comment_value.has_value());
    EXPECT_EQ(comment_value.value(), " Comment with    lots   of    spaces    ");
}

// Test no whitespace at all
TEST(TokenizerTest, NoWhitespace) {
    auto tokens = tokenize_all(
        "{\"key\":42,\"array\":[true,false,null],\"nested\":{\"inner\":3.14}}"
    );
    
    ASSERT_EQ(tokens.size(), 23);
    // Verify all tokens are correctly parsed despite lack of whitespace
    check_token_type(tokens[0], jayson::token_type::OBJECT_BEGIN);
    check_token_type(tokens[1], jayson::token_type::STRING); // "key"
    check_token_type(tokens[2], jayson::token_type::COLON);
    check_token_type(tokens[3], jayson::token_type::INTEGER); // 42
    check_token_type(tokens[4], jayson::token_type::COMMA);
    check_token_type(tokens[5], jayson::token_type::STRING); // "array"
    check_token_type(tokens[6], jayson::token_type::COLON);
    check_token_type(tokens[7], jayson::token_type::ARRAY_BEGIN);
    check_token_type(tokens[8], jayson::token_type::BOOLEAN); // true
    check_token_type(tokens[9], jayson::token_type::COMMA);
    check_token_type(tokens[10], jayson::token_type::BOOLEAN); // false
    check_token_type(tokens[11], jayson::token_type::COMMA);
    check_token_type(tokens[12], jayson::token_type::NONE); // null
    check_token_type(tokens[13], jayson::token_type::ARRAY_END);
    check_token_type(tokens[14], jayson::token_type::COMMA);
    check_token_type(tokens[15], jayson::token_type::STRING); // "nested"
    check_token_type(tokens[16], jayson::token_type::COLON);
    check_token_type(tokens[17], jayson::token_type::OBJECT_BEGIN);
    check_token_type(tokens[18], jayson::token_type::STRING);
    check_token_type(tokens[19], jayson::token_type::COLON);
    check_token_type(tokens[20], jayson::token_type::FLOAT);
    check_token_type(tokens[21], jayson::token_type::OBJECT_END);
    check_token_type(tokens[22], jayson::token_type::OBJECT_END);
}

// Test whitespace handling with comments
TEST(TokenizerTest, WhitespaceWithComments) {
    auto tokens = tokenize_all(
        "  {  // Comment at the start\n"
        "     \"key\"  :  // Comment after colon\n"
        "     42  // Comment after value\n"
        "  }  // Comment at the end\n"
    );
    
    ASSERT_EQ(tokens.size(), 9);
    
    // Verify tokens with comments interspersed
    check_token_type(tokens[0], jayson::token_type::OBJECT_BEGIN);
    check_token_type(tokens[1], jayson::token_type::COMMENT); // Comment after {
    check_token_type(tokens[2], jayson::token_type::STRING); // "key"
    check_token_type(tokens[3], jayson::token_type::COLON);
    check_token_type(tokens[4], jayson::token_type::COMMENT); // Comment after :
    check_token_type(tokens[5], jayson::token_type::INTEGER); // 42
    check_token_type(tokens[6], jayson::token_type::COMMENT); // Comment after 42
    check_token_type(tokens[7], jayson::token_type::OBJECT_END);
    check_token_type(tokens[8], jayson::token_type::COMMENT);
}


// Test recovery from malformed number
TEST(TokenizerTest, RecoveryFromMalformedNumber) {
    auto tokenizer = jayson::tokenize("42.1.2 true");
    
    // should parse 42.1
    auto token1 = tokenizer.get_next_token();
    EXPECT_TRUE(token1.has_value());
    
    // should fail to parse .2
    auto token2 = tokenizer.get_next_token();
    ASSERT_FALSE(token2.has_value());
}

// Test recovery from valid tokens followed by invalid json structure
TEST(TokenizerTest, RecoveryFromInvalidJsonStructure) {
    auto tokenizer = jayson::tokenize("[1, 2, 3} 42");
    
    // Array begin should be valid
    auto token1 = tokenizer.get_next_token();
    ASSERT_TRUE(token1.has_value());
    EXPECT_EQ(token1.value().get_type(), jayson::token_type::ARRAY_BEGIN);
    
    // 1 should be valid
    auto token2 = tokenizer.get_next_token();
    ASSERT_TRUE(token2.has_value());
    EXPECT_EQ(token2.value().get_type(), jayson::token_type::INTEGER);
    
    // Comma should be valid
    auto token3 = tokenizer.get_next_token();
    ASSERT_TRUE(token3.has_value());
    EXPECT_EQ(token3.value().get_type(), jayson::token_type::COMMA);
    
    // 2 should be valid
    auto token4 = tokenizer.get_next_token();
    ASSERT_TRUE(token4.has_value());
    EXPECT_EQ(token4.value().get_type(), jayson::token_type::INTEGER);
    
    // Comma should be valid
    auto token5 = tokenizer.get_next_token();
    ASSERT_TRUE(token5.has_value());
    EXPECT_EQ(token5.value().get_type(), jayson::token_type::COMMA);
    
    // 3 should be valid
    auto token6 = tokenizer.get_next_token();
    ASSERT_TRUE(token6.has_value());
    EXPECT_EQ(token6.value().get_type(), jayson::token_type::INTEGER);
    
    // } should be valid as a token (though structurally invalid for JSON)
    auto token7 = tokenizer.get_next_token();
    ASSERT_TRUE(token7.has_value());
    EXPECT_EQ(token7.value().get_type(), jayson::token_type::OBJECT_END);
    
    // 42 should be valid
    auto token8 = tokenizer.get_next_token();
    ASSERT_TRUE(token8.has_value());
    EXPECT_EQ(token8.value().get_type(), jayson::token_type::INTEGER);
}

// Test floating point with variety of exponent notations
TEST(TokenizerTest, ExponentNotations) {
    std::vector<std::pair<std::string, double>> exponents = {
        {"1e5", 1e5},
        {"1E5", 1e5},
        {"1.0e5", 1.0e5},
        {"1.0E5", 1.0e5},
        {"1.e5", 1.0e5},
        {"1.E5", 1.0e5},
        {"1e+5", 1e+5},
        {"1E+5", 1e+5},
        {"1.0e+5", 1.0e+5},
        {"1.0E+5", 1.0e+5},
        {"1.e+5", 1.0e+5},
        {"1.E+5", 1.0e+5},
        {"1e-5", 1e-5},
        {"1E-5", 1e-5},
        {"1.0e-5", 1.0e-5},
        {"1.0E-5", 1.0e-5},
        {"1.e-5", 1.0e-5},
        {"1.E-5", 1.0e-5}
    };
    
    for (const auto& [input, expected] : exponents) {
        auto tokens = tokenize_all(input);
        ASSERT_EQ(tokens.size(), 1) << "Failed on input: " << input;
        check_float_token(tokens[0], expected);
    }
}

// Test boundary between integer and float
TEST(TokenizerTest, IntegerFloatBoundary) {
    // Float with integer value
    auto tokens3 = tokenize_all("123.0");
    ASSERT_EQ(tokens3.size(), 1);
    EXPECT_EQ(tokens3[0].get_type(), jayson::token_type::FLOAT);
}

