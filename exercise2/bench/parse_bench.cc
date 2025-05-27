#include <benchmark/benchmark.h>
#include <string>
#include <sstream>
#include <fstream>

#include "jayson.hpp"
#include "jayson_fixed.hpp"
#include "jayson_bench_helper.hpp"

using namespace jayson::bench;

//------------------------------------------------------------------------------
// BASIC VALUE TYPE BENCHMARKS
//------------------------------------------------------------------------------

static void parse_strings(benchmark::State &state) {
    std::string test_string = generate_random_strings(1);
    for (auto _ : state) {
        auto result = jayson::parse_direct(test_string);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_strings);

static void parse_integers(benchmark::State &state) {
    for (auto _ : state) {
        auto result = jayson::parse_direct("42");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_integers);

static void parse_floats(benchmark::State &state) {
    for (auto _ : state) {
        auto result = jayson::parse_direct("3.14159");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_floats);

static void parse_scientific_notation(benchmark::State &state) {
    for (auto _ : state) {
        auto result = jayson::parse_direct("1.23e-4");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_scientific_notation);

static void parse_booleans(benchmark::State &state) {
    for (auto _ : state) {
        auto result = jayson::parse_direct("true");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_booleans);

static void parse_null(benchmark::State &state) {
    for (auto _ : state) {
        auto result = jayson::parse_direct("null");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_null);

//------------------------------------------------------------------------------
// STRUCTURE TYPE BENCHMARKS
//------------------------------------------------------------------------------

static void parse_empty_object(benchmark::State &state) {
    for (auto _ : state) {
        auto result = jayson::parse_direct("{}");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_empty_object);

static void parse_empty_array(benchmark::State &state) {
    for (auto _ : state) {
        auto result = jayson::parse_direct("[]");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_empty_array);

static void parse_small_object(benchmark::State &state) {
    std::string json = generate_random_object(5);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_small_object);

static void parse_medium_object(benchmark::State &state) {
    std::string json = generate_random_object(50);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_medium_object);

static void parse_large_object(benchmark::State &state) {
    std::string json = generate_random_object(500);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_large_object);

static void parse_small_array(benchmark::State &state) {
    std::string json = generate_random_array(10);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_small_array);

static void parse_medium_array(benchmark::State &state) {
    std::string json = generate_random_array(100);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_medium_array);

static void parse_large_array(benchmark::State &state) {
    std::string json = generate_random_array(1000);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_large_array);

//------------------------------------------------------------------------------
// NESTED STRUCTURE BENCHMARKS
//------------------------------------------------------------------------------

static void parse_shallow_nesting(benchmark::State &state) {
    std::string json = generate_complex_json(3, 3);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_shallow_nesting);

static void parse_medium_nesting(benchmark::State &state) {
    std::string json = generate_complex_json(5, 3);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_medium_nesting);

static void parse_deep_nesting(benchmark::State &state) {
    std::string json = generate_complex_json(10, 2);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_deep_nesting);

static void parse_complex_json(benchmark::State &state) {
    std::string json = generate_complex_json(4, 4);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_complex_json);

//------------------------------------------------------------------------------
// REALISTIC JSON BENCHMARKS
//------------------------------------------------------------------------------

static void parse_realistic_json(benchmark::State &state) {
    std::string json = generate_realistic_json();
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_realistic_json);

static void parse_api_response_simulation(benchmark::State &state) {
    // clang-format off
    std::string json = R"({
        "status": "success",
        "data": {
            "users": [)" +
                generate_realistic_json() + "," +
                generate_realistic_json() + "," +
                generate_realistic_json() + R"(],
            "pagination": {
                "page": 1,
                "per_page": 3,
                "total": 150,
                "has_more": true
            }
        },
        "meta": {
            "timestamp": "2023-03-15T12:00:00Z",
            "version": "v1.2.3",
            "request_id": "req_abc123def456"
        }
    })";
    // clang-format on

    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_api_response_simulation);

static void parse_config_file_simulation(benchmark::State &state) {
    std::string json = R"({
        "app": {
            "name": "MyApplication",
            "version": "2.1.0",
            "environment": "production"
        },
        "database": {
            "primary": {
                "host": "db-primary.example.com",
                "port": 5432,
                "name": "myapp_prod",
                "ssl": true,
                "pool_size": 20,
                "timeout": 30000
            },
            "replica": {
                "host": "db-replica.example.com", 
                "port": 5432,
                "name": "myapp_prod",
                "ssl": true,
                "pool_size": 10,
                "timeout": 30000
            }
        },
        "cache": {
            "redis": {
                "nodes": [
                    {"host": "redis-1.example.com", "port": 6379},
                    {"host": "redis-2.example.com", "port": 6379},
                    {"host": "redis-3.example.com", "port": 6379}
                ],
                "ttl": 3600,
                "key_prefix": "myapp:"
            }
        },
        "logging": {
            "level": "info",
            "format": "json",
            "outputs": ["stdout", "file"],
            "file_path": "/var/log/myapp.log",
            "max_size": "100MB",
            "max_files": 10
        },
        "features": {
            "new_ui": true,
            "beta_features": false,
            "rate_limiting": {
                "enabled": true,
                "requests_per_minute": 100
            },
            "authentication": {
                "jwt_secret": "secret_key_here",
                "token_expiry": 86400,
                "refresh_token_expiry": 604800
            }
        }
    })";

    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_config_file_simulation);

//------------------------------------------------------------------------------
// REAL WORLD FILE BENCHMARK
//------------------------------------------------------------------------------

static void parse_red_dress_json(benchmark::State &state) {
    // Load the real-world JSON file
    std::ifstream t("tests/red-dress.jayson");
    if (!t.is_open()) {
        state.SkipWithError("Could not open tests/red-dress.jayson");
        return;
    }

    std::stringstream buffer;
    buffer << t.rdbuf();
    auto input = buffer.str();

    if (input.empty()) {
        state.SkipWithError("red-dress.jayson is empty or could not be read");
        return;
    }

    for (auto _ : state) {
        auto result = jayson::parse_direct(input);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_red_dress_json);

//------------------------------------------------------------------------------
// FORMATTING VARIATION BENCHMARKS
//------------------------------------------------------------------------------

static void parse_compact_json(benchmark::State &state) {
    std::string json =
        "{\"name\":\"John\",\"age\":30,\"active\":true,\"scores\":[85,92,78],"
        "\"address\":{\"city\":\"NYC\",\"zip\":\"10001\"}}";
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_compact_json);

static void parse_formatted_json(benchmark::State &state) {
    std::string json = R"({
  "name": "John",
  "age": 30,
  "active": true,
  "scores": [
    85,
    92,
    78
  ],
  "address": {
    "city": "NYC",
    "zip": "10001"
  }
})";
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_formatted_json);

static void parse_json_with_comments(benchmark::State &state) {
    std::string json = R"({
  // User information
  "name": "John", // Full name
  "age": 30, // Age in years
  "active": true, // Account status
  "scores": [ // Test scores
    85, // Math
    92, // Science  
    78  // English
  ],
  "address": { // Home address
    "city": "NYC", // City name
    "zip": "10001" // Postal code
  }
  // End of user data
})";
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_json_with_comments);

static void parse_whitespace_heavy_json(benchmark::State &state) {
    std::string base_json = generate_random_object(20);
    std::string json      = generate_whitespace_json(base_json);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_whitespace_heavy_json);

//------------------------------------------------------------------------------
// STRING PROCESSING BENCHMARKS
//------------------------------------------------------------------------------

static void parse_long_strings(benchmark::State &state) {
    std::string json = generate_long_string(1000);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_long_strings);

static void parse_many_small_strings(benchmark::State &state) {
    std::stringstream ss;
    ss << "[";
    for (int i = 0; i < 100; ++i) {
        if (i > 0) ss << ",";
        ss << "\"string" << i << "\"";
    }
    ss << "]";
    std::string json = ss.str();

    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_many_small_strings);

//------------------------------------------------------------------------------
// NUMERIC PROCESSING BENCHMARKS
//------------------------------------------------------------------------------

static void parse_many_integers(benchmark::State &state) {
    std::stringstream ss;
    ss << "[";
    for (int i = 0; i < 1000; ++i) {
        if (i > 0) ss << ",";
        ss << (i * 1234567);
    }
    ss << "]";
    std::string json = ss.str();

    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_many_integers);

static void parse_many_floats(benchmark::State &state) {
    std::stringstream ss;
    ss << "[";
    for (int i = 0; i < 500; ++i) {
        if (i > 0) ss << ",";
        ss << (i * 3.14159) << "e" << (i % 10 - 5);
    }
    ss << "]";
    std::string json = ss.str();

    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_many_floats);

//------------------------------------------------------------------------------
// MEMORY ALLOCATION PATTERNS
//------------------------------------------------------------------------------

static void parse_wide_object_many_keys(benchmark::State &state) {
    std::string json = generate_random_object(1000);
    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_wide_object_many_keys);

static void parse_deep_array_nesting(benchmark::State &state) {
    std::string json;
    int depth = 50;
    for (int i = 0; i < depth; ++i) { json += "["; }
    json += "42";
    for (int i = 0; i < depth; ++i) { json += "]"; }

    for (auto _ : state) {
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(parse_deep_array_nesting);

//------------------------------------------------------------------------------
// COMPARATIVE BENCHMARKS
//------------------------------------------------------------------------------

static void tokenize_only(benchmark::State &state) {
    std::string json = generate_realistic_json();
    for (auto _ : state) {
        // Just tokenize, don't parse
        auto tokenizer = jayson::tokenize(json);
        while (tokenizer.get_next_token().has_value()) {}
        benchmark::DoNotOptimize(tokenizer);
    }
}
BENCHMARK(tokenize_only);

static void tokenize_and_parse(benchmark::State &state) {
    std::string json = generate_realistic_json();
    for (auto _ : state) {
        // Full parse pipeline
        auto result = jayson::parse(jayson::tokenize(json));
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(tokenize_and_parse);
static void tokenize_and_parse_direct(benchmark::State &state) {
    std::string json = generate_realistic_json();
    for (auto _ : state) {
        // Full parse pipeline
        auto result = jayson::parse_direct(json);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(tokenize_and_parse_direct);

BENCHMARK_MAIN();
