#ifndef INCLUDED_JAYSON_DUMP_HPP
#define INCLUDED_JAYSON_DUMP_HPP

#include "jayson.hpp"
#include <algorithm>
#include <cassert>
#include <string>

namespace jayson {

constexpr inline auto indent_depth = 2;

std::string to_json(const jayson_element *source, int indent = 0);

namespace detail {

    inline auto to_json_string(string_type str) {
        return "\"" + std::string {str} + "\"";
    }

    inline std::string make_indent(int indent, bool newline = true) {
        auto result = std::string {};
        result.reserve(indent + newline);
        if (newline) result += '\n';
        for (int i = 0; i < indent; ++i) result += ' ';
        return result;
    }

    inline std::string object_to_json(const jayson_object *source, int indent) {
        assert(source);
        if (source->size() == 0) return "{}";
        auto inner_depth   = indent + indent_depth;
        std::string result = "{";
        bool first         = true;
        auto keys          = source->get_keys();
        std::sort(keys.begin(), keys.end());
        for (const auto &el : keys) {
            if (!first)
                result += ',';
            else
                first = false;
            result += make_indent(inner_depth) + to_json_string(el) + ": " +
                      to_json(source->get_value_for(el), inner_depth);
        }
        result += make_indent(indent);
        result += '}';
        return result;
    }
    inline std::string array_to_json(const jayson_array *source, int indent) {
        assert(source);
        if (source->size() == 0) return "[]";
        auto inner_depth   = indent + indent_depth;
        std::string result = "[";
        bool first         = true;
        for (const auto &el : source->get_elements()) {
            if (!first)
                result += ',';
            else
                first = false;
            result += make_indent(inner_depth) + to_json(el, inner_depth);
        }
        result += make_indent(indent);
        result += ']';
        return result;
    }
    inline std::string string_to_json(const jayson_string *source) {
        assert(source);
        return to_json_string(source->get_string());
    }
    inline std::string integer_to_json(const jayson_integer *source) {
        assert(source);
        return std::to_string(source->get_integer());
    }
    inline std::string float_to_json(const jayson_float *source) {
        assert(source);
        return std::to_string(source->get_float());
    }
    inline std::string boolean_to_json(const jayson_boolean *source) {
        assert(source);
        return source->get_boolean() ? "true" : "false";
    }
    inline std::string none_to_json(const jayson_none *source) {
        assert(source);
        return "null";
    }
} // namespace detail


inline std::string to_json(const jayson_element *source, int indent) {
    assert(source);

    switch (source->get_type()) {
    case jayson_types::OBJECT: return detail::object_to_json(source->to_object(), indent);
    case jayson_types::ARRAY: return detail::array_to_json(source->to_array(), indent);
    case jayson_types::STRING: return detail::string_to_json(source->to_string());
    case jayson_types::INTEGER: return detail::integer_to_json(source->to_integer());
    case jayson_types::FLOAT: return detail::float_to_json(source->to_float());
    case jayson_types::BOOLEAN: return detail::boolean_to_json(source->to_boolean());
    case jayson_types::NONE: return detail::none_to_json(source->to_none());
    }
    assert(!"This switch should have no exit than to return. "
        "If you've ventured here and looked into the abyss, the abyss is staring back "
        "(and also crashing your program)");
    return "";
}

} // namespace jayson

#endif