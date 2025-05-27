#ifndef INCLUDED_JAYSON_PRINT_HPP
#define INCLUDED_JAYSON_PRINT_HPP

#include "jayson.hpp"
#include "jayson_fixed.hpp"
#include <cassert>
#include <ostream>
#include <iostream>

namespace jayson {

template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(std::basic_ostream<Char_T, Char_Traits> &str, token_type type) {
    str << "[token_type: ";
    switch (type) {
    case token_type::OBJECT_BEGIN: return str << "OBJECT_BEGIN" << "]";
    case token_type::OBJECT_END: return str << "OBJECT_END" << "]";
    case token_type::ARRAY_BEGIN: return str << "ARRAY_BEGIN" << "]";
    case token_type::ARRAY_END: return str << "ARRAY_END" << "]";
    case token_type::COMMA: return str << "COMMA" << "]";
    case token_type::COLON: return str << "COLON" << "]";
    case token_type::NONE: return str << "NONE" << "]";
    case token_type::STRING: return str << "STRING" << "]";
    case token_type::INTEGER: return str << "INTEGER" << "]";
    case token_type::FLOAT: return str << "FLOAT" << "]";
    case token_type::BOOLEAN: return str << "BOOLEAN" << "]";
    case token_type::COMMENT: return str << "COMMENT" << "]";
    }
    assert(false);
    return str;
}
template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, jayson_types type) {
    str << "[jayson_types: ";
    switch (type) {
    case jayson_types::OBJECT: return str << "OBJECT" << "]";
    case jayson_types::ARRAY: return str << "ARRAY" << "]";
    case jayson_types::STRING: return str << "STRING" << "]";
    case jayson_types::INTEGER: return str << "INTEGER" << "]";
    case jayson_types::FLOAT: return str << "FLOAT" << "]";
    case jayson_types::BOOLEAN: return str << "BOOLEAN" << "]";
    case jayson_types::NONE: return str << "NONE" << "]";
    }
    assert(false);
    return str;
}

template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const token &t) {
    str << "[token: " << t.get_type() << ", [original: " << t.get_original() << "]";
    switch (t.get_type()) {
    case token_type::STRING: str << ", [string: " << t.get_string().value() << "]"; break;
    case token_type::INTEGER: str << ", [int: " << t.get_integer().value() << "]"; break;
    case token_type::FLOAT: str << ", [float: " << t.get_float().value() << "]"; break;
    case token_type::BOOLEAN: str << ", [boolean: " << t.get_boolean().value() << "]"; break;
    case token_type::COMMENT: str << ", [comment: " << t.get_comment().value() << "]"; break;
    default: break;
    }
    return str << "]";
}

template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_object &value) {
    str << "[object: [size: " << value.size() << "], [values: ";
    for (auto &key : value.get_keys()) {
        str << "[\"" << key << "\": " << *value.get_value_for(key) << "@" << value.get_value_for(key) << "], ";
    }
    return str << "]]";
}
template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_array &value) {
    str << "[array: [size: " << value.size() << "], [values: ";
    for (auto element : value.get_elements()) { str << "[" << *element << "@" << element << "], "; }
    return str << "]]";
}
template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_string &value) {
    return str << "[string: " << value.get_string() << "]";
}
template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_integer &value) {
    return str << "[integer: " << value.get_integer() << "]";
}
template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_float &value) {
    return str << "[float: " << value.get_float() << "]";
}
template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_boolean &value) {
    return str << "[boolean: " << (value.get_boolean() ? "true" : "false") << "]";
}
template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_none &) {
    return str << "[none]";
}

template<typename Char_T, typename Char_Traits>
std::basic_ostream<Char_T, Char_Traits> &operator<<(
    std::basic_ostream<Char_T, Char_Traits> &str, const jayson_element &value) {
    str << "[element: " << value.get_type() << ", " << std::flush;
    switch (value.get_type()) {
    case jayson_types::OBJECT: assert(value.to_object()); return str << *value.to_object() << "]";
    case jayson_types::ARRAY: assert(value.to_array()); return str << *value.to_array() << "]";
    case jayson_types::STRING: assert(value.to_string()); return str << *value.to_string() << "]";
    case jayson_types::INTEGER:
        assert(value.to_integer());
        return str << *value.to_integer() << "]";
    case jayson_types::FLOAT: assert(value.to_float()); return str << *value.to_float() << "]";
    case jayson_types::BOOLEAN:
        assert(value.to_boolean());
        return str << *value.to_boolean() << "]";
    case jayson_types::NONE: assert(value.to_none()); return str << *value.to_none() << "]";
    }
    assert(false);
    return str;
}

inline void dump_tokens(const std::vector<token> &tokens) {
    for (const auto &token : tokens) { std::cout << token << std::endl; }
}

} // namespace jayson
#endif
