#ifndef INCLUDED_JAYSON_TESTHELPER_HPP
#define INCLUDED_JAYSON_TESTHELPER_HPP

#include "jayson_dump.hpp"

// Helper function to get all tokens from a string
inline std::vector<jayson::token> tokenize_all(std::string_view input) {
    auto tokenizer = jayson::tokenize(input);
    std::vector<jayson::token> tokens;

    while (auto token = tokenizer.get_next_token()) { tokens.push_back(token.value()); }

    return tokens;
}

// Helper to check token type
inline void check_token_type(const jayson::token &token, jayson::token_type expected_type) {
    EXPECT_EQ(token.get_type(), expected_type);
}

// Helper to check string token
inline void check_string_token(const jayson::token &token, const std::string &expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::STRING);
    auto value = token.get_string();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}

// Helper to check integer token
inline void check_integer_token(const jayson::token &token, jayson::integer_type expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::INTEGER);
    auto value = token.get_integer();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}

// Helper to check float token
inline void check_float_token(const jayson::token &token, jayson::float_type expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::FLOAT);
    auto value = token.get_float();
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), expected_value);
}

// Helper to check boolean token
inline void check_boolean_token(const jayson::token &token, bool expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::BOOLEAN);
    auto value = token.get_boolean();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}

// Helper to check comment token
inline void check_comment_token(const jayson::token &token, const std::string &expected_value) {
    EXPECT_EQ(token.get_type(), jayson::token_type::COMMENT);
    auto value = token.get_comment();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), expected_value);
}


template<typename Lambda>
inline auto test_parse_both(std::string_view input, Lambda &&l) {
    using namespace std::literals;
    auto tokenized = jayson::parse(jayson::tokenize(input));
    l("tokenized"s, std::move(tokenized));
    auto direct = jayson::parse_direct(input);
    l("direct"s, std::move(direct));
}


// Helper function to verify parsing fails
inline void check_parse_fails(std::string_view input) {
    test_parse_both(input, [&](auto mode, auto result) {
        if (result) { std::cout << to_json(result.get()) << std::endl; }
        EXPECT_FALSE(result) << "Expected " << mode << " parsing to fail for: " << input;
    });
}


// Helper function to parse and verify a string value
inline void check_string_value(std::string_view input, const std::string &expected_value) {
    test_parse_both(input, [&](auto mode, auto result) {
        ASSERT_TRUE(result) << "Failed to " << mode << " parse: " << input;
        EXPECT_EQ(result->get_type(), jayson::jayson_types::STRING);

        auto string_obj = result->to_string();
        ASSERT_TRUE(string_obj);
        EXPECT_EQ(string_obj->get_string(), expected_value);
    });
}

// Helper function to parse and verify an integer value
inline void check_integer_value(std::string_view input, jayson::integer_type expected_value) {
    test_parse_both(input, [&](auto mode, auto result) {
        ASSERT_TRUE(result) << "Failed to " << mode << " parse: " << input;
        EXPECT_EQ(result->get_type(), jayson::jayson_types::INTEGER);

        auto int_obj = result->to_integer();
        ASSERT_TRUE(int_obj);
        EXPECT_EQ(int_obj->get_integer(), expected_value);
    });
}

// Helper function to parse and verify a float value
inline void check_float_value(std::string_view input, jayson::float_type expected_value) {
    test_parse_both(input, [&](auto mode, auto result) {
        ASSERT_TRUE(result) << "Failed to " << mode << " parse: " << input;
        EXPECT_EQ(result->get_type(), jayson::jayson_types::FLOAT);

        auto float_obj = result->to_float();
        ASSERT_TRUE(float_obj);
        EXPECT_DOUBLE_EQ(float_obj->get_float(), expected_value);
    });
}

// Helper function to parse and verify a boolean value
inline void check_boolean_value(std::string_view input, bool expected_value) {
    test_parse_both(input, [&](auto mode, auto result) {
        ASSERT_TRUE(result) << "Failed to " << mode << " parse: " << input;
        EXPECT_EQ(result->get_type(), jayson::jayson_types::BOOLEAN);

        auto bool_obj = result->to_boolean();
        ASSERT_TRUE(bool_obj);
        EXPECT_EQ(bool_obj->get_boolean(), expected_value);
    });
}

// Helper function to parse and verify a null value
inline void check_null_value(std::string_view input) {
    test_parse_both(input, [&](auto mode, auto result) {
        ASSERT_TRUE(result) << "Failed to " << mode << " parse: " << input;
        EXPECT_EQ(result->get_type(), jayson::jayson_types::NONE);

        auto null_obj = result->to_none();
        ASSERT_TRUE(null_obj);
    });
}


#endif
