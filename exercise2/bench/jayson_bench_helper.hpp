#ifndef INCLUDED_JAYSON_BENCH_HELPER_HPP
#define INCLUDED_JAYSON_BENCH_HELPER_HPP

#include <string>
#include <random>
#include <array>
#include <sstream>
#include <cassert>

namespace jayson::bench {

constexpr auto bench_bytes = 2 * 1024 * 1024; // 2MB of data for benchmarks

// Helper function to repeat a string n times
inline std::string repeat(const std::string &str, size_t n) {
    std::string result;
    result.reserve(str.size() * n);
    for (size_t i = 0; i < n; ++i) { result += str; }
    return result;
}

// Helper function to pad a string to target size
inline std::string make_test_string(const std::string &base, std::size_t target_size = bench_bytes) {
    std::string result;
    result.reserve(target_size + base.size());
    while (result.size() < target_size) { result += base; }
    return result;
}

// Random generators with fixed seeds for reproducibility
inline std::mt19937 get_generator() {
    return std::mt19937(42); // Fixed seed for reproducibility
}

// Generate random integer strings
inline std::string generate_random_integers(size_t count) {
    auto gen = get_generator();
    std::uniform_int_distribution<int> dist(-1000000, 1000000);

    std::stringstream ss;
    for (size_t i = 0; i < count; ++i) {
        ss << dist(gen);
        if (i < count - 1) ss << ' ';
    }
    return ss.str();
}

// Generate random float strings
inline std::string generate_random_floats(size_t count) {
    auto gen = get_generator();
    std::uniform_real_distribution<double> dist(-1000.0, 1000.0);
    std::uniform_int_distribution<int> exp_dist(-10, 10);

    std::stringstream ss;
    for (size_t i = 0; i < count; ++i) {
        double value = dist(gen);
        if (i % 5 == 0 && i > 0) {
            // Add some exponential notation
            int exp = exp_dist(gen);
            ss << value << "e" << exp;
        } else {
            ss << value;
        }
        if (i < count - 1) ss << ' ';
    }
    return ss.str();
}

// Generate random strings
inline std::string generate_random_strings(size_t count, size_t avg_length = 20) {
    auto gen = get_generator();
    std::uniform_int_distribution<size_t> len_dist(5, avg_length * 2);
    std::uniform_int_distribution<char> char_dist('a', 'z');

    std::stringstream ss;
    for (size_t i = 0; i < count; ++i) {
        size_t length = len_dist(gen);
        ss << '"';
        for (size_t j = 0; j < length; ++j) { ss << char_dist(gen); }
        ss << '"';
        if (i < count - 1) ss << ' ';
    }
    return ss.str();
}

// Generate random keys (for objects)
inline std::string generate_random_key(std::mt19937 &gen, size_t avg_length = 10) {
    std::uniform_int_distribution<size_t> len_dist(5, avg_length * 2);
    std::uniform_int_distribution<char> char_dist('A', 'Z');

    size_t length   = len_dist(gen);
    std::string key = "\"";
    for (size_t j = 0; j < length; ++j) { key += char_dist(gen); }
    key += "\"";
    return key;
}

// Generate random JSON objects
inline std::string generate_random_object(size_t num_keys) {
    auto gen = get_generator();
    std::uniform_int_distribution<int> value_type_dist(0, 4); // 0=int, 1=float, 2=string, 3=bool, 4=null
    std::uniform_int_distribution<int> int_dist(-1000, 1000);
    std::uniform_real_distribution<double> float_dist(-1000.0, 1000.0);
    std::uniform_int_distribution<char> char_dist('a', 'z');
    std::uniform_int_distribution<int> bool_dist(0, 1);
    std::uniform_int_distribution<size_t> str_len_dist(5, 20);

    std::stringstream ss;
    ss << "{";

    for (size_t i = 0; i < num_keys; ++i) {
        std::string key = generate_random_key(gen);
        ss << key << ": ";

        int type = value_type_dist(gen);
        switch (type) {
        case 0: // Integer
            ss << int_dist(gen);
            break;
        case 1: // Float
            ss << float_dist(gen);
            break;
        case 2: { // String
            size_t str_len = str_len_dist(gen);
            ss << "\"";
            for (size_t j = 0; j < str_len; ++j) { ss << char_dist(gen); }
            ss << "\"";
            break;
        }
        case 3: // Boolean
            ss << (bool_dist(gen) ? "true" : "false");
            break;
        case 4: // Null
            ss << "null";
            break;
        }

        if (i < num_keys - 1) { ss << ", "; }
    }

    ss << "}";
    return ss.str();
}

// Generate random JSON arrays
inline std::string generate_random_array(size_t num_elements) {
    auto gen = get_generator();
    std::uniform_int_distribution<int> value_type_dist(0, 4); // 0=int, 1=float, 2=string, 3=bool, 4=null
    std::uniform_int_distribution<int> int_dist(-1000, 1000);
    std::uniform_real_distribution<double> float_dist(-1000.0, 1000.0);
    std::uniform_int_distribution<char> char_dist('a', 'z');
    std::uniform_int_distribution<int> bool_dist(0, 1);
    std::uniform_int_distribution<size_t> str_len_dist(5, 20);

    std::stringstream ss;
    ss << "[";

    int type = value_type_dist(gen);
    for (size_t i = 0; i < num_elements; ++i) {
        switch (type) {
        case 0: // Integer
            ss << int_dist(gen);
            break;
        case 1: // Float
            ss << float_dist(gen);
            break;
        case 2: { // String
            size_t str_len = str_len_dist(gen);
            ss << "\"";
            for (size_t j = 0; j < str_len; ++j) { ss << char_dist(gen); }
            ss << "\"";
            break;
        }
        case 3: // Boolean
            ss << (bool_dist(gen) ? "true" : "false");
            break;
        case 4: // Null
            ss << "null";
            break;
        }

        if (i < num_elements - 1) { ss << ", "; }
    }

    ss << "]";
    return ss.str();
}

// Generate a complex nested JSON with objects and arrays
inline std::string generate_complex_json(size_t depth = 3, size_t width = 5) {
    if (depth == 0) {
        auto gen = get_generator();
        std::uniform_int_distribution<int> type_dist(0, 4);
        std::uniform_int_distribution<int> int_dist(-1000, 1000);
        std::uniform_real_distribution<double> float_dist(-1000.0, 1000.0);
        std::uniform_int_distribution<char> char_dist('a', 'z');
        std::uniform_int_distribution<size_t> str_len_dist(5, 20);

        int type = type_dist(gen);
        switch (type) {
        case 0: // Integer
            return std::to_string(int_dist(gen));
        case 1: // Float
            return std::to_string(float_dist(gen));
        case 2: { // String
            size_t str_len     = str_len_dist(gen);
            std::string result = "\"";
            for (size_t j = 0; j < str_len; ++j) { result += char_dist(gen); }
            result += "\"";
            return result;
        }
        case 3: // Boolean
            return type_dist(gen) % 2 == 0 ? "true" : "false";
        case 4: // Null
            return "null";
        default: return "null";
        }
    }

    auto gen = get_generator();
    std::uniform_int_distribution<int> type_dist(0, 1); // 0=object, 1=array

    int type = type_dist(gen);
    if (type == 0) { // Object
        std::stringstream ss;
        ss << "{";

        for (size_t i = 0; i < width; ++i) {
            std::string key = generate_random_key(gen);
            ss << key << ": " << generate_complex_json(depth - 1, width);

            if (i < width - 1) { ss << ", "; }
        }

        ss << "}";
        return ss.str();
    } else { // Array
        std::stringstream ss;
        ss << "[";

        for (size_t i = 0; i < width; ++i) {
            ss << generate_complex_json(depth - 1, width);

            if (i < width - 1) { ss << ", "; }
        }

        ss << "]";
        return ss.str();
    }
}

// Generate JSON with lots of whitespace
inline std::string generate_whitespace_json(const std::string &base_json) {
    auto gen = get_generator();
    std::uniform_int_distribution<int> ws_type_dist(0, 3); // 0=space, 1=tab, 2=newline, 3=carriage return
    std::uniform_int_distribution<int> ws_count_dist(1, 5);

    std::stringstream ss;
    for (char c : base_json) {
        ss << c;

        // Add whitespace after structural characters
        if (c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ':') {
            int ws_count = ws_count_dist(gen);
            for (int i = 0; i < ws_count; ++i) {
                int ws_type = ws_type_dist(gen);
                switch (ws_type) {
                case 0: ss << ' '; break;
                case 1: ss << '\t'; break;
                case 2: ss << '\n'; break;
                case 3: ss << '\r'; break;
                }
            }
        }
    }

    return ss.str();
}

// Generate a very long string
inline std::string generate_long_string(size_t length) {
    auto gen = get_generator();
    std::uniform_int_distribution<char> char_dist('a', 'z');

    std::string result = "\"";
    result.reserve(length + 2);

    for (size_t i = 0; i < length; ++i) { result += char_dist(gen); }

    result += "\"";
    return result;
}

// Generate comments
inline std::string generate_comments(size_t count, size_t avg_length = 30) {
    auto gen = get_generator();
    std::uniform_int_distribution<size_t> len_dist(10, avg_length * 2);
    std::uniform_int_distribution<char> char_dist('a', 'z');

    std::stringstream ss;
    for (size_t i = 0; i < count; ++i) {
        size_t length = len_dist(gen);
        ss << "// ";
        for (size_t j = 0; j < length; ++j) { ss << char_dist(gen); }
        ss << "\n";
    }
    return ss.str();
}

// Generate realistic JSON
inline std::string generate_realistic_json() {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"id\": " << std::to_string(1234567890) << ",\n";
    ss << "  \"name\": \"" << "JaySon Test Data" << "\",\n";
    ss << "  \"version\": " << "1.0" << ",\n";
    ss << "  \"active\": true,\n";
    ss << "  \"dependencies\": " << generate_random_array(10) << ",\n";
    ss << "  \"config\": " << generate_random_object(15) << ",\n";
    ss << "  // Configuration options\n";
    ss << "  \"settings\": {\n";
    ss << "    \"debug\": true,\n";
    ss << "    \"timeout\": 30,\n";
    ss << "    \"retries\": 3,\n";
    ss << "    \"paths\": " << generate_random_array(5) << "\n";
    ss << "  },\n";
    ss << "  \"nested\": " << generate_complex_json(4, 3) << "\n";
    ss << "}";

    return ss.str();
}

} // namespace jayson::bench

#endif // INCLUDED_JAYSON_BENCH_HELPER_HPP
