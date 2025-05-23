// TODO fix includes
#include "../include/jayson_dump.hpp"
#include "../include/jayson_print.hpp"
#include "../include/jayson_comparators.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string_view>

int main(int argc, char **argv) {
    // cheat here and use stdin directly instead of cin
    // (because we would either need an object or a reference which does not work)
    auto file  = argc > 1 ? std::ifstream {argv[1]} : std::ifstream {"/dev/stdin"};
    auto input = std::string();
    while (true) {
        char c = file.get();
        if (!file)
            break;
        else
            input += c;
    }
    std::cout << "tokenizing:" << std::endl;
    auto tokenized = jayson::tokenize(std::string_view {input.c_str(), input.size()});
    for (auto token = tokenized.get_next_token(); token; token = tokenized.get_next_token()) {
        std::cout << token.value() << std::endl;
    }
    std::cout << "parsing:" << std::endl;
    auto parsed = jayson::parse_direct(std::string_view {input.c_str(), input.size()});
    assert(parsed);
    std::cout << *parsed.get() << std::endl;
    std::cout << "pretty-printed" << std::endl;
    std::cout << jayson::to_json(parsed.get()) << std::endl;
}