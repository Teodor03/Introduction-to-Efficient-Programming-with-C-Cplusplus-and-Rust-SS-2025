#ifndef INCLUDED_JAYSON_FIXED_HPP
#define INCLUDED_JAYSON_FIXED_HPP

#include <memory>
#include <string_view>

namespace jayson {

enum class token_type {
    // unvalued tokens
    OBJECT_BEGIN, // {
    OBJECT_END,   // }
    ARRAY_BEGIN,  // [
    ARRAY_END,    // ]
    COMMA,        // ,
    COLON,        // :
    NONE,         // null
    // tokens with a value
    STRING,  // "this is an (ASCII-only) string"
    INTEGER, // 123456
    FLOAT,   // 123.456
    BOOLEAN, // true
    COMMENT, // // and the string behind it as its value
};

struct token;
struct tokenizer;
[[nodiscard]] tokenizer tokenize(std::string_view input);


enum class jayson_types {
    OBJECT,
    ARRAY,
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    NONE,
    // no comments here any more
};

struct jayson_element;
struct jayson_object;
struct jayson_array;
struct jayson_string;
struct jayson_integer;
struct jayson_float;
struct jayson_boolean;
struct jayson_none;


[[nodiscard]] std::unique_ptr<jayson_element> parse(tokenizer tokens);
[[nodiscard]] std::unique_ptr<jayson_element> parse_direct(std::string_view input);

} // namespace jayson

#endif