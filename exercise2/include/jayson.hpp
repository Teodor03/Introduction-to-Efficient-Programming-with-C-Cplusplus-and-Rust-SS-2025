#ifndef INCLUDED_JAYSON_HPP
#define INCLUDED_JAYSON_HPP

#include "jayson_fixed.hpp"

#include <cstdint>
#include <optional>
#include <vector>
#include <unordered_map>

namespace jayson {

using integer_type = std::int64_t;
using float_type   = double;
using string_type  = std::string_view;

union token_parsed_value {
    integer_type int_value;
    float_type float_value;
    string_type string_value;
    bool boolean_value;

    explicit token_parsed_value()
        : int_value(0) {
    }

    explicit token_parsed_value(const integer_type &int_value)
        : int_value(int_value) {
    }

    explicit token_parsed_value(const float_type &float_value)
        : float_value(float_value) {
    }

    explicit token_parsed_value(const string_type &string_value)
        : string_value(string_value) {
    }

    explicit token_parsed_value(const bool &boolean_value)
        : boolean_value(boolean_value) {
    }
};

struct token {

    token(const token_type &type, const string_type &original_string)
        : type(type),
          original_string(original_string) {
    }

    token(const token_type &type, const string_type &original_string, const integer_type &int_value)
        : type(type),
          original_string(original_string),
          parsed_value(int_value) {
    }

    token(const token_type &type, const string_type &original_string, const float_type &float_value)
    : type(type),
      original_string(original_string),
      parsed_value(float_value) {
    }

    token(const token_type &type, const string_type &original_string, const string_type &string_value)
    : type(type),
      original_string(original_string),
      parsed_value(string_value) {
    }

    token(const token_type &type, const string_type &original_string, const bool &boolean_value)
    : type(type),
      original_string(original_string),
      parsed_value(boolean_value) {
    }


    [[nodiscard]] token_type get_type() const;
    [[nodiscard]] string_type get_original() const;
    // no getter for single-character tokens
    [[nodiscard]] std::optional<string_type> get_string() const;
    [[nodiscard]] std::optional<string_type> get_comment() const;
    [[nodiscard]] std::optional<bool> get_boolean() const;
    // no get_none() as there is nothing to get
    [[nodiscard]] std::optional<integer_type> get_integer() const;
    [[nodiscard]] std::optional<float_type> get_float() const;

private:

    token_type type;
    string_type original_string;
    token_parsed_value parsed_value;

};

struct tokenizer {

    explicit tokenizer(const string_type &input)
        : input(input),
          pos(0) {
    }

    [[nodiscard]] std::optional<token> get_next_token();
    [[nodiscard]] std::optional<token> peek_next_token() const;

private:

    string_type input;
    size_t pos;

};

struct jayson_element_base {

    virtual ~jayson_element_base() = default;
    [[nodiscard]] virtual jayson_types get_type() const = 0;

};

struct jayson_object : jayson_element_base {

    std::unordered_map<string_type, std::unique_ptr<jayson_element>> map;

    [[nodiscard]] jayson_types get_type() const override;
    [[nodiscard]] integer_type size() const;
    [[nodiscard]] std::vector<string_type> get_keys() const;
    [[nodiscard]] std::vector<const jayson_element *> get_values() const;
    [[nodiscard]] const jayson_element *get_value_for(const string_type &key) const;
};

struct jayson_array : jayson_element_base {

    std::vector<std::unique_ptr<jayson_element>> array;

    [[nodiscard]] jayson_types get_type() const override;
    [[nodiscard]] integer_type size() const;
    [[nodiscard]] std::vector<const jayson_element *> get_elements() const;
    [[nodiscard]] const jayson_element *get_value_at(integer_type index) const;
};

struct jayson_string : jayson_element_base{

    string_type string;

    explicit jayson_string(const string_type& value);
    [[nodiscard]] jayson_types get_type() const override;
    [[nodiscard]] string_type get_string() const;
};

struct jayson_integer : jayson_element_base {

    integer_type integer;

    explicit jayson_integer(const integer_type& value);
    [[nodiscard]] jayson_types get_type() const override;
    [[nodiscard]] integer_type get_integer() const;
};

struct jayson_float : jayson_element_base {

    float_type floating;

    explicit jayson_float(const float_type& value);
    [[nodiscard]] jayson_types get_type() const override;
    [[nodiscard]] float_type get_float() const;
};

struct jayson_boolean : jayson_element_base{

    bool boolean;

    explicit jayson_boolean(const bool& value);
    [[nodiscard]] jayson_types get_type() const override;
    [[nodiscard]] bool get_boolean() const;
};

struct jayson_none : jayson_element_base {
    [[nodiscard]] jayson_types get_type() const override;
};

struct jayson_element {

    explicit jayson_element(std::unique_ptr<jayson_element_base> element);
    [[nodiscard]] jayson_types get_type() const;
    [[nodiscard]] const jayson_object *to_object() const;
    [[nodiscard]] const jayson_array *to_array() const;
    [[nodiscard]] const jayson_string *to_string() const;
    [[nodiscard]] const jayson_integer *to_integer() const;
    [[nodiscard]] const jayson_float *to_float() const;
    [[nodiscard]] const jayson_boolean *to_boolean() const;
    [[nodiscard]] const jayson_none *to_none() const;

private:

    std::unique_ptr<jayson_element_base> element;

};

} // namespace jayson

#endif