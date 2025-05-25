#include "../include/jayson.hpp"


jayson::token_type jayson::token::get_type() const {
    return jayson::token_type::NONE;
}

jayson::string_type jayson::token::get_original() const {
    return "";
}

std::optional<jayson::string_type> jayson::token::get_string() const {
    return {};
}

std::optional<jayson::integer_type> jayson::token::get_integer() const {
    return {};
}

std::optional<jayson::float_type> jayson::token::get_float() const {
    return {};
}

std::optional<bool> jayson::token::get_boolean() const {
    return {};
}

std::optional<jayson::string_type> jayson::token::get_comment() const {
    return {};
}

std::optional<jayson::token> jayson::tokenizer::get_next_token() {
    return {};
}

std::optional<jayson::token> jayson::tokenizer::peek_next_token() const {
    return {};
}

jayson::tokenizer jayson::tokenize(std::string_view input) {
    (void)input;
    return {};
}




jayson::integer_type jayson::jayson_object::size() const {
    return 0;
}

std::vector<jayson::string_type> jayson::jayson_object::get_keys() const {
    return {};
}

std::vector<const jayson::jayson_element *> jayson::jayson_object::get_values() const {
    return {};
}

const jayson::jayson_element *jayson::jayson_object::get_value_for(const jayson::string_type &key) const {
    (void)key;
    return 0;
}

jayson::integer_type jayson::jayson_array::size() const {
    return 0;
}

std::vector<const jayson::jayson_element *> jayson::jayson_array::get_elements() const {
    return {};
}

const jayson::jayson_element *jayson::jayson_array::get_value_at(jayson::integer_type index) const {
    (void)index;
    return 0;
}

jayson::string_type jayson::jayson_string::get_string() const {
    return "";
}

jayson::integer_type jayson::jayson_integer::get_integer() const {
    return 0;
}

jayson::float_type jayson::jayson_float::get_float() const {
    return 0.0;
}

bool jayson::jayson_boolean::get_boolean() const {
    return false;
}



jayson::jayson_types jayson::jayson_element::get_type() const {
    return jayson_types::NONE;
}

const jayson::jayson_object *jayson::jayson_element::to_object() const {
    return 0;
}

const jayson::jayson_array *jayson::jayson_element::to_array() const {
    return 0;
}

const jayson::jayson_string *jayson::jayson_element::to_string() const {
    return 0;
}

const jayson::jayson_integer *jayson::jayson_element::to_integer() const {
    return 0;
}

const jayson::jayson_float *jayson::jayson_element::to_float() const {
    return 0;
}

const jayson::jayson_boolean *jayson::jayson_element::to_boolean() const {
    return 0;
}

const jayson::jayson_none *jayson::jayson_element::to_none() const {
    return 0;
}


std::unique_ptr<jayson::jayson_element> jayson::parse(tokenizer tokens) {
    (void)tokens;
    return {};
}

std::unique_ptr<jayson::jayson_element> jayson::parse_direct(std::string_view input) {
    return parse(tokenize(input));
}
