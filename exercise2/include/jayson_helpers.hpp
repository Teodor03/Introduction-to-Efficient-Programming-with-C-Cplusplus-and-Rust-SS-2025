#ifndef INCLUDED_JAYSON_HELPERS_HPP
#define INCLUDED_JAYSON_HELPERS_HPP

#include <cctype>
namespace jayson::helpers {

constexpr inline bool is_whitespace(char input) {
    return std::isspace(static_cast<unsigned char>(input));
}
constexpr inline bool is_digit(char input) {
    return std::isdigit(static_cast<unsigned char>(input));
}

} // namespace jayson::helpers

#endif