#ifndef INCLUDED_JAYSON_HPP
#define INCLUDED_JAYSON_HPP

#include "jayson_fixed.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace jayson {

using integer_type = std::int64_t;
using float_type   = double;
using string_type  = std::string_view;

struct token {
    [[nodiscard]] token_type get_type() const;
    [[nodiscard]] string_type get_original() const;
    // no getter for single-character tokens
    [[nodiscard]] std::optional<string_type> get_string() const;
    [[nodiscard]] std::optional<string_type> get_comment() const;
    [[nodiscard]] std::optional<bool> get_boolean() const;
    // no get_none() as there is nothing to get
    [[nodiscard]] std::optional<integer_type> get_integer() const;
    [[nodiscard]] std::optional<float_type> get_float() const;
};

struct tokenizer {
    [[nodiscard]] std::optional<token> get_next_token();
    [[nodiscard]] std::optional<token> peek_next_token() const;
};


struct jayson_object {
    [[nodiscard]] integer_type size() const;
    // get_value_for(get_keys()[i]) == get_values()[i]
    [[nodiscard]] std::vector<string_type> get_keys() const;
    [[nodiscard]] std::vector<const jayson_element *> get_values() const;
    [[nodiscard]] const jayson_element *get_value_for(const string_type &key) const;
};
struct jayson_array {
    [[nodiscard]] integer_type size() const;
    [[nodiscard]] std::vector<const jayson_element *> get_elements() const;
    [[nodiscard]] const jayson_element *get_value_at(integer_type index) const;
};
struct jayson_string {
    [[nodiscard]] string_type get_string() const;
};
struct jayson_integer {
    [[nodiscard]] integer_type get_integer() const;
};
struct jayson_float {
    [[nodiscard]] float_type get_float() const;
};
struct jayson_boolean {
    [[nodiscard]] bool get_boolean() const;
};
struct jayson_none {};

struct jayson_element {
    [[nodiscard]] jayson_types get_type() const;
    [[nodiscard]] const jayson_object *to_object() const;
    [[nodiscard]] const jayson_array *to_array() const;
    [[nodiscard]] const jayson_string *to_string() const;
    [[nodiscard]] const jayson_integer *to_integer() const;
    [[nodiscard]] const jayson_float *to_float() const;
    [[nodiscard]] const jayson_boolean *to_boolean() const;
    [[nodiscard]] const jayson_none *to_none() const;
};

} // namespace jayson

#endif