#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "jayson_fixed.hpp"
#include "jayson.hpp"
#include "../bench/jayson_bench_helper.hpp"

using namespace jayson::bench;

// Helper function to get all tokens from a string
std::vector<jayson::token> tokenize_all(std::string_view input) {
    auto tokenizer = jayson::tokenize(input);
    std::vector<jayson::token> tokens;

    while (auto token = tokenizer.get_next_token()) { tokens.push_back(token.value()); }

    return tokens;
}

// Test that verifies the benchmark for structural tokens
TEST(TokenizeBenchTest, StructuralTokens) {
    std::string test_string = "{}[]:,";
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::OBJECT_BEGIN);
    EXPECT_EQ(tokens[1].get_type(), jayson::token_type::OBJECT_END);
    EXPECT_EQ(tokens[2].get_type(), jayson::token_type::ARRAY_BEGIN);
    EXPECT_EQ(tokens[3].get_type(), jayson::token_type::ARRAY_END);
    EXPECT_EQ(tokens[4].get_type(), jayson::token_type::COLON);
    EXPECT_EQ(tokens[5].get_type(), jayson::token_type::COMMA);
}

// Test that verifies the benchmark for string tokens
TEST(TokenizeBenchTest, StringTokens) {
    std::string test_string = generate_random_strings(10);
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 10);
    for (const auto &token : tokens) {
        EXPECT_EQ(token.get_type(), jayson::token_type::STRING);
        EXPECT_TRUE(token.get_string().has_value());
    }
}

// Test that verifies the benchmark for integer tokens
TEST(TokenizeBenchTest, IntegerTokens) {
    std::string test_string = generate_random_integers(10);
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 10);
    for (const auto &token : tokens) {
        EXPECT_EQ(token.get_type(), jayson::token_type::INTEGER);
        EXPECT_TRUE(token.get_integer().has_value());
    }
}

// Test that verifies the benchmark for float tokens
TEST(TokenizeBenchTest, FloatTokens) {
    std::string test_string = generate_random_floats(10);
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 10);
    for (const auto &token : tokens) {
        EXPECT_TRUE(token.get_type() == jayson::token_type::FLOAT ||
                    token.get_type() == jayson::token_type::INTEGER);
    }
}

// Test that verifies the benchmark for boolean tokens
TEST(TokenizeBenchTest, BooleanTokens) {
    std::string test_string = "true false";
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::BOOLEAN);
    EXPECT_EQ(tokens[1].get_type(), jayson::token_type::BOOLEAN);
    EXPECT_TRUE(tokens[0].get_boolean().has_value());
    EXPECT_TRUE(tokens[1].get_boolean().has_value());
    EXPECT_TRUE(tokens[0].get_boolean().value());
    EXPECT_FALSE(tokens[1].get_boolean().value());
}

// Test that verifies the benchmark for null tokens
TEST(TokenizeBenchTest, NullTokens) {
    std::string test_string = "null";
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::NONE);
}

// Test that verifies the benchmark for comment tokens
TEST(TokenizeBenchTest, CommentTokens) {
    std::string test_string = generate_comments(5);
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 5);
    for (const auto &token : tokens) {
        EXPECT_EQ(token.get_type(), jayson::token_type::COMMENT);
        EXPECT_TRUE(token.get_comment().has_value());
    }
}

// Test that verifies the benchmark for excessive whitespace
TEST(TokenizeBenchTest, ExcessiveWhitespace) {
    std::string base_json   = "{\"a\": 1}";
    std::string test_string = generate_whitespace_json(base_json);
    auto tokens             = tokenize_all(test_string);

    // We expect at least these tokens
    ASSERT_GE(tokens.size(), 5);

    // Check the key tokens (ignoring potential whitespace-related differences)
    bool found_object_begin = false;
    bool found_string       = false;
    bool found_colon        = false;
    bool found_integer      = false;
    bool found_object_end   = false;

    for (const auto &token : tokens) {
        if (token.get_type() == jayson::token_type::OBJECT_BEGIN) found_object_begin = true;
        if (token.get_type() == jayson::token_type::STRING) found_string = true;
        if (token.get_type() == jayson::token_type::COLON) found_colon = true;
        if (token.get_type() == jayson::token_type::INTEGER) found_integer = true;
        if (token.get_type() == jayson::token_type::OBJECT_END) found_object_end = true;
    }

    EXPECT_TRUE(found_object_begin);
    EXPECT_TRUE(found_string);
    EXPECT_TRUE(found_colon);
    EXPECT_TRUE(found_integer);
    EXPECT_TRUE(found_object_end);
}

// Test that verifies the benchmark for no whitespace
TEST(TokenizeBenchTest, NoWhitespace) {
    std::string test_string = "{\"a\":1,\"b\":2,\"c\":3,\"d\":true,\"e\":null,\"f\":\"string\"}";
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 25); // 6 key-value pairs * 3 tokens each + { and } + 5 * ,
    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::OBJECT_BEGIN);
    EXPECT_EQ(tokens[tokens.size() - 1].get_type(), jayson::token_type::OBJECT_END);
}

// Test that verifies the benchmark for nested objects
TEST(TokenizeBenchTest, NestedObjects) {
    std::string test_string = generate_complex_json(3, 2);
    auto tokens             = tokenize_all(test_string);

    // Just verify that we get some tokens without errors
    EXPECT_GT(tokens.size(), 10);

    // Check for balance of braces/brackets
    int object_balance = 0;
    int array_balance  = 0;

    for (const auto &token : tokens) {
        if (token.get_type() == jayson::token_type::OBJECT_BEGIN) object_balance++;
        if (token.get_type() == jayson::token_type::OBJECT_END) object_balance--;
        if (token.get_type() == jayson::token_type::ARRAY_BEGIN) array_balance++;
        if (token.get_type() == jayson::token_type::ARRAY_END) array_balance--;
    }

    EXPECT_EQ(object_balance, 0);
    EXPECT_EQ(array_balance, 0);
}

// Test that verifies the benchmark for large arrays
TEST(TokenizeBenchTest, LargeArrays) {
    std::string test_string = generate_random_array(20);
    auto tokens             = tokenize_all(test_string);

    // Minimum tokens: [ + 20 elements + 19 commas + ]
    ASSERT_GE(tokens.size(), 41);

    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::ARRAY_BEGIN);
    EXPECT_EQ(tokens[tokens.size() - 1].get_type(), jayson::token_type::ARRAY_END);
}

// Test that verifies the benchmark for wide objects
TEST(TokenizeBenchTest, WideObjects) {
    std::string test_string = generate_random_object(20);
    auto tokens             = tokenize_all(test_string);

    // Minimum tokens: { + 20 keys + 20 colons + 20 values + 19 commas + }
    ASSERT_GE(tokens.size(), 81);

    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::OBJECT_BEGIN);
    EXPECT_EQ(tokens[tokens.size() - 1].get_type(), jayson::token_type::OBJECT_END);
}

// Test that verifies the benchmark for token extraction vs tokenization (strings)
TEST(TokenizeBenchTest, TokenGetVsExtractString) {
    std::string test_string = generate_random_strings(10);
    auto tokenizer          = jayson::tokenize(test_string);

    std::optional<jayson::token> token;
    while ((token = tokenizer.get_next_token()).has_value()) {
        if (token->get_type() == jayson::token_type::STRING) {
            auto string_value = token->get_string();
            EXPECT_TRUE(string_value.has_value());
            EXPECT_FALSE(string_value.value().empty());
        }
    }
}

// Test that verifies the benchmark for token extraction vs tokenization (numbers)
TEST(TokenizeBenchTest, TokenGetVsExtractNumber) {
    std::string test_string = generate_random_integers(5) + " " + generate_random_floats(5);
    auto tokenizer          = jayson::tokenize(test_string);

    std::optional<jayson::token> token;
    while ((token = tokenizer.get_next_token()).has_value()) {
        if (token->get_type() == jayson::token_type::INTEGER) {
            auto int_value = token->get_integer();
            EXPECT_TRUE(int_value.has_value());
        } else if (token->get_type() == jayson::token_type::FLOAT) {
            auto float_value = token->get_float();
            EXPECT_TRUE(float_value.has_value());
        }
    }
}

// Test that verifies the benchmark for peek vs get
TEST(TokenizeBenchTest, PeekVsGet) {
    std::string test_string = generate_realistic_json();
    auto tokenizer          = jayson::tokenize(test_string);

    while (true) {
        // First peek
        auto peek_result = tokenizer.peek_next_token();
        if (!peek_result.has_value()) break;

        // Then get
        auto get_result = tokenizer.get_next_token();
        ASSERT_TRUE(get_result.has_value());

        // Check that peek and get return the same token type
        EXPECT_EQ(peek_result->get_type(), get_result->get_type());

        // For string tokens, check they have the same value
        if (peek_result->get_type() == jayson::token_type::STRING) {
            EXPECT_EQ(peek_result->get_string(), get_result->get_string());
        }
    }
}

// Test that verifies the benchmark for interleaved peek and get
TEST(TokenizeBenchTest, InterleavedPeekGet) {
    std::string test_string = generate_realistic_json();
    auto tokenizer          = jayson::tokenize(test_string);
    int counter             = 0;

    while (true) {
        std::optional<jayson::token> token;
        if (counter % 2 == 0) {
            token = tokenizer.get_next_token();
        } else {
            auto peek = tokenizer.peek_next_token();
            if (peek.has_value()) {
                token = tokenizer.get_next_token();
                // Ensure peek and get give the same token
                EXPECT_EQ(peek->get_type(), token->get_type());
            } else {
                break;
            }
        }
        if (!token.has_value()) break;
        counter++;
    }

    // We should have processed some tokens
    EXPECT_GT(counter, 0);
}

// Test that verifies the benchmark for multiple peeks
TEST(TokenizeBenchTest, MultiplePeeks) {
    std::string test_string = generate_realistic_json();
    auto tokenizer          = jayson::tokenize(test_string);

    while (true) {
        // Peek 3 times
        auto peek1 = tokenizer.peek_next_token();
        if (!peek1.has_value()) break;

        auto peek2 = tokenizer.peek_next_token();
        auto peek3 = tokenizer.peek_next_token();

        // All peeks should return the same token
        ASSERT_TRUE(peek2.has_value());
        ASSERT_TRUE(peek3.has_value());
        EXPECT_EQ(peek1->get_type(), peek2->get_type());
        EXPECT_EQ(peek1->get_type(), peek3->get_type());

        // Then get
        auto get_result = tokenizer.get_next_token();
        ASSERT_TRUE(get_result.has_value());
        EXPECT_EQ(peek1->get_type(), get_result->get_type());
    }
}

// Test that verifies the benchmark for very long strings
TEST(TokenizeBenchTest, VeryLongStrings) {
    std::string test_string = generate_long_string(1000);
    auto tokens             = tokenize_all(test_string);

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].get_type(), jayson::token_type::STRING);
    EXPECT_TRUE(tokens[0].get_string().has_value());
    EXPECT_EQ(tokens[0].get_string().value().size(), 1000);
}

// Test that verifies the benchmark for realistic JSON
TEST(TokenizeBenchTest, RealisticJson) {
    std::string test_string = generate_realistic_json();
    auto tokens             = tokenize_all(test_string);

    // We should have a significant number of tokens
    EXPECT_GT(tokens.size(), 50);

    // Check for balance of braces/brackets
    int object_balance = 0;
    int array_balance  = 0;

    for (const auto &token : tokens) {
        if (token.get_type() == jayson::token_type::OBJECT_BEGIN) object_balance++;
        if (token.get_type() == jayson::token_type::OBJECT_END) object_balance--;
        if (token.get_type() == jayson::token_type::ARRAY_BEGIN) array_balance++;
        if (token.get_type() == jayson::token_type::ARRAY_END) array_balance--;
    }

    EXPECT_EQ(object_balance, 0);
    EXPECT_EQ(array_balance, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
