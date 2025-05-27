#include <benchmark/benchmark.h>

#include "jayson_fixed.hpp"
#include "jayson.hpp"
#include "jayson_bench_helper.hpp"

using namespace jayson::bench;

// Benchmark for tokenizing structural tokens ({}, [], :, ,)
static void tokenize_structure(benchmark::State &state) {
    std::string test_string = make_test_string("{}[]:,");
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_structure);

// Benchmark for tokenizing string tokens
static void tokenize_strings(benchmark::State &state) {
    std::string test_string = make_test_string(generate_random_strings(100));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_strings);

// Benchmark for tokenizing integer tokens
static void tokenize_integers(benchmark::State &state) {
    std::string test_string = make_test_string(generate_random_integers(100));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_integers);

// Benchmark for tokenizing float tokens
static void tokenize_floats(benchmark::State &state) {
    std::string test_string = make_test_string(generate_random_floats(100));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_floats);

// Benchmark for tokenizing boolean tokens
static void tokenize_booleans(benchmark::State &state) {
    std::string test_string = make_test_string("true false ");
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_booleans);

// Benchmark for tokenizing null tokens
static void tokenize_nulls(benchmark::State &state) {
    std::string test_string = make_test_string("null ");
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_nulls);

// Benchmark for tokenizing comment tokens
static void tokenize_comments(benchmark::State &state) {
    std::string test_string = make_test_string(generate_comments(50));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_comments);

// Benchmark for JSON with excessive whitespace
static void tokenize_excessive_whitespace(benchmark::State &state) {
    std::string base_json   = generate_random_object(20);
    std::string test_string = make_test_string(generate_whitespace_json(base_json));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_excessive_whitespace);

// Benchmark for JSON with no whitespace
static void tokenize_no_whitespace(benchmark::State &state) {
    std::string test_string =
        make_test_string("{\"a\":1,\"b\":2,\"c\":3,\"d\":true,\"e\":null,\"f\":\"string\"}");
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_no_whitespace);

// Benchmark for nested objects
static void tokenize_nested_objects(benchmark::State &state) {
    std::string test_string = make_test_string(generate_complex_json(5, 3));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_nested_objects);

// Benchmark for large arrays
static void tokenize_large_arrays(benchmark::State &state) {
    std::string test_string = make_test_string(generate_random_array(100));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_large_arrays);

// Benchmark for wide objects (many key-value pairs)
static void tokenize_wide_objects(benchmark::State &state) {
    std::string test_string = make_test_string(generate_random_object(100));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_wide_objects);

// Benchmark for comparing token extraction and tokenization (strings)
static void token_get_and_extract_string(benchmark::State &state) {
    std::string test_string = make_test_string(generate_random_strings(100));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        std::optional<jayson::token> token;
        while ((token = tokenizer.get_next_token()).has_value()) {
            if (token->get_type() == jayson::token_type::STRING) {
                benchmark::DoNotOptimize(token->get_string());
            }
        }
    }
}
BENCHMARK(token_get_and_extract_string);

// Benchmark for comparing token extraction and tokenization (numbers)
static void token_get_and_extract_number(benchmark::State &state) {
    std::string test_string =
        make_test_string(generate_random_integers(50) + " " + generate_random_floats(50));
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        std::optional<jayson::token> token;
        while ((token = tokenizer.get_next_token()).has_value()) {
            if (token->get_type() == jayson::token_type::INTEGER) {
                benchmark::DoNotOptimize(token->get_integer());
            } else if (token->get_type() == jayson::token_type::FLOAT) {
                benchmark::DoNotOptimize(token->get_float());
            }
        }
    }
}
BENCHMARK(token_get_and_extract_number);

// Benchmark for interleaved peek and get operations
static void interleaved_peek_get(benchmark::State &state) {
    std::string test_string = make_test_string(generate_realistic_json());
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        int counter    = 0;
        while (true) {
            std::optional<jayson::token> token;
            if (counter % 2 == 0) {
                token = tokenizer.get_next_token();
            } else {
                auto peek = tokenizer.peek_next_token();
                if (peek.has_value()) {
                    token = tokenizer.get_next_token();
                } else {
                    break;
                }
            }
            if (!token.has_value()) break;
            counter++;
        }
    }
}
BENCHMARK(interleaved_peek_get);

// Benchmark for multiple peeks in a row
static void multiple_peeks(benchmark::State &state) {
    std::string test_string = make_test_string(generate_realistic_json());
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (true) {
            // Peek 3 times
            auto peek1 = tokenizer.peek_next_token();
            if (!peek1.has_value()) break;

            benchmark::DoNotOptimize(tokenizer.peek_next_token());
            benchmark::DoNotOptimize(tokenizer.peek_next_token());

            // Then get
            benchmark::DoNotOptimize(tokenizer.get_next_token());
        }
    }
}
BENCHMARK(multiple_peeks);

// Benchmark for very long string tokens
static void tokenize_very_long_strings(benchmark::State &state) {
    std::string test_string = make_test_string(generate_long_string(10000) + " ");
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_very_long_strings);

// Benchmark for realistic JSON
static void tokenize_realistic_json(benchmark::State &state) {
    std::string test_string = make_test_string(generate_realistic_json());
    for (auto _ : state) {
        auto tokenizer = jayson::tokenize(test_string);
        while (tokenizer.get_next_token().has_value()) {}
    }
}
BENCHMARK(tokenize_realistic_json);

BENCHMARK_MAIN();
