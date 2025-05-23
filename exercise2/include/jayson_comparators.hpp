#ifndef INCLUDED_JAYSON_COMPARATORS_HPP
#define INCLUDED_JAYSON_COMPARATORS_HPP

#include "jayson.hpp"
#include <stdbool.h>

namespace jayson {

inline bool operator==(const token &fst, const token &scn) {
    return fst.get_type() == scn.get_type() && fst.get_original() == scn.get_original() &&
           fst.get_string() == scn.get_string() && fst.get_integer() == scn.get_integer() &&
           fst.get_float() == scn.get_float() && fst.get_boolean() == scn.get_boolean() &&
           fst.get_boolean() == scn.get_boolean();
}
inline bool operator!=(const token &fst, const token &scn) {
    return !(fst == scn);
}

inline bool operator==(const jayson_object &fst, const jayson_object &scn) {
    return fst.size() == scn.size() && fst.get_keys() == scn.get_keys() &&
           fst.get_values() == scn.get_values();
}
inline bool operator!=(const jayson_object &fst, const jayson_object &scn) {
    return !(fst == scn);
}
inline bool operator==(const jayson_array &fst, const jayson_array &scn) {
    return fst.size() == scn.size() && fst.get_elements() == scn.get_elements();
}
inline bool operator!=(const jayson_array &fst, const jayson_array &scn) {
    return !(fst == scn);
}
inline bool operator==(const jayson_string &fst, const jayson_string &scn) {
    return fst.get_string() == scn.get_string();
}
inline bool operator!=(const jayson_string &fst, const jayson_string &scn) {
    return !(fst == scn);
}
inline bool operator==(const jayson_integer &fst, const jayson_integer &scn) {
    return fst.get_integer() == scn.get_integer();
}
inline bool operator!=(const jayson_integer &fst, const jayson_integer &scn) {
    return !(fst == scn);
}
inline bool operator==(const jayson_float &fst, const jayson_float &scn) {
    return fst.get_float() == scn.get_float();
}
inline bool operator!=(const jayson_float &fst, const jayson_float &scn) {
    return !(fst == scn);
}
inline bool operator==(const jayson_boolean &fst, const jayson_boolean &scn) {
    return fst.get_boolean() == scn.get_boolean();
}
inline bool operator!=(const jayson_boolean &fst, const jayson_boolean &scn) {
    return !(fst == scn);
}


inline bool operator==(const jayson_element &fst, const jayson_element &scn) {
    return fst.get_type() == scn.get_type() && fst.to_object() == scn.to_object() &&
           fst.to_array() == scn.to_array() && fst.to_string() == scn.to_string() &&
           fst.to_integer() == scn.to_integer() && fst.to_float() == scn.to_float() &&
           fst.to_boolean() == scn.to_boolean() && fst.to_none() == scn.to_none();
}

} // namespace jayson

#endif