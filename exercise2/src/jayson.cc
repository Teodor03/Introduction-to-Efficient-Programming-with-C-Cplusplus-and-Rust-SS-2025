#include "../include/jayson.hpp"

jayson::token_type jayson::token::get_type() const {
    return type;
}

jayson::string_type jayson::token::get_original() const {
    return original_string;
}

std::optional<jayson::string_type> jayson::token::get_string() const {
    if (type == jayson::token_type::STRING)
        return this->parsed_value.string_value;
    return std::nullopt;
}

std::optional<jayson::integer_type> jayson::token::get_integer() const {
    if (type == jayson::token_type::INTEGER)
        return this->parsed_value.int_value;
    return std::nullopt;
}

std::optional<jayson::float_type> jayson::token::get_float() const {
    if (type == jayson::token_type::FLOAT)
        return this->parsed_value.float_value;
    return std::nullopt;
}

std::optional<bool> jayson::token::get_boolean() const {
    if (type == jayson::token_type::BOOLEAN)
        return this->parsed_value.boolean_value;
    return std::nullopt;
}

std::optional<jayson::string_type> jayson::token::get_comment() const {
    if (type == jayson::token_type::COMMENT)
        return this->parsed_value.string_value;
    return std::nullopt;
}

std::optional<jayson::token> jayson::tokenizer::get_next_token() {
    auto next_token = this->peek_next_token();
    if (!next_token.has_value())
        return std::nullopt;
    this->pos = static_cast<size_t>(next_token.value().get_original().data() - this->input.data()) + next_token.value().
                get_original().size();
    return next_token;
}

std::optional<jayson::token> jayson::tokenizer::peek_next_token() const {
    size_t current_pos = this->pos;
    size_t next_token_starting_pos;
    string_type token_string;
    string_type string_value;

    while (current_pos < this->input.size()) {
        switch (this->input[current_pos]) {
            case ' ':
                break;
            case '\n':
                break;
            case '\r':
                break;
            case '\t':
                break;
            case '{':
                token_string = string_type(this->input.data() + current_pos, 1);
                return token(token_type::OBJECT_BEGIN, token_string);
            case '}':
                token_string = string_type(this->input.data() + current_pos, 1);
                return token(token_type::OBJECT_END, token_string);
            case '[':
                token_string = string_type(this->input.data() + current_pos, 1);
                return token(token_type::ARRAY_BEGIN, token_string);
            case ']':
                token_string = string_type(this->input.data() + current_pos, 1);
                return token(token_type::ARRAY_END, token_string);
            case ',':
                token_string = string_type(this->input.data() + current_pos, 1);
                return token(token_type::COMMA, token_string);
            case ':':
                token_string = string_type(this->input.data() + current_pos, 1);
                return token(token_type::COLON, token_string);
            case '"':
                next_token_starting_pos = current_pos;
                current_pos++;
                while (current_pos != this->input.size()) {
                    if (this->input[current_pos] == '"') {
                        token_string = string_type(this->input.data() + next_token_starting_pos,
                                                        current_pos - next_token_starting_pos + 1);
                        string_value = string_type(this->input.data() + next_token_starting_pos + 1,
                                                        current_pos - next_token_starting_pos - 1);
                        return token(token_type::STRING, token_string, string_value);
                    }
                    current_pos++;
                }
                return std::nullopt;
            case '/':
                if (current_pos == (this->input.size() - 1))
                    return std::nullopt;
                if (this->input[current_pos + 1] != '/')
                    return std::nullopt;
                next_token_starting_pos = current_pos;
                while (current_pos != (this->input.size() - 1) && this->input[current_pos] != '\n') {
                    current_pos++;
                }
                token_string = string_type(this->input.data() + next_token_starting_pos,
                                                current_pos - next_token_starting_pos + 1);
                string_value = string_type(this->input.data() + next_token_starting_pos + 2,
                                                current_pos - next_token_starting_pos - (
                                                    this->input[current_pos] == '\n' ? 2 : 1));
                return token(token_type::COMMENT, token_string, string_value);
            case 't':
                if (current_pos + 4 > this->input.size())
                    return std::nullopt;
                if (this->input[current_pos + 1] == 'r' &&
                    this->input[current_pos + 2] == 'u' &&
                    this->input[current_pos + 3] == 'e') {
                    token_string = string_type(this->input.data() + current_pos, 4);
                    return token(token_type::BOOLEAN, token_string, true);
                }
                return std::nullopt;
            case 'f':
                if (current_pos + 5 > this->input.size())
                    return std::nullopt;
                if (this->input[current_pos + 1] == 'a' &&
                    this->input[current_pos + 2] == 'l' &&
                    this->input[current_pos + 3] == 's' &&
                    this->input[current_pos + 4] == 'e') {
                    token_string = string_type(this->input.data() + current_pos, 5);
                    return token(token_type::BOOLEAN, token_string, false);
                }
                return std::nullopt;
            case 'n':
                if (current_pos + 4 > this->input.size())
                    return std::nullopt;
                if (this->input[current_pos + 1] == 'u' &&
                    this->input[current_pos + 2] == 'l' &&
                    this->input[current_pos + 3] == 'l') {
                    token_string = string_type(this->input.data() + current_pos, 4);
                    return token(token_type::NONE, token_string);
                }
                return std::nullopt;
            default:
                //Parse numbers.
                next_token_starting_pos = current_pos;
                //Parse optional negative sign.
                if (this->input[current_pos] == '-')
                    current_pos++;
                //Parse decimal part.
                if (this->input[current_pos] == '0') {
                    current_pos++;
                } else if (this->input[current_pos] >= '1' && this->input[current_pos] <= '9') {
                    current_pos++;
                    while (this->input[current_pos] >= '0' && this->input[current_pos] <= '9') {
                        current_pos++;
                    }
                } else {
                    return std::nullopt;
                }
                //Parse fraction part.
                bool is_integer = true;
                if (this->input[current_pos] == '.') {
                    is_integer = false;
                    current_pos++;
                    if (this->input[current_pos] < '0' || this->input[current_pos] > '9')
                        return std::nullopt;
                    current_pos++;
                    while (this->input[current_pos] >= '0' && this->input[current_pos] <= '9') {
                        current_pos++;
                    }
                }
                //Parse exponent.
                if (this->input[current_pos] == 'e' || this->input[current_pos] == 'E') {
                    is_integer = false;
                    current_pos++;
                    if (this->input[current_pos] == '-' || this->input[current_pos] == '+')
                        current_pos++;
                    if (this->input[current_pos] < '0' || this->input[current_pos] > '9')
                        return std::nullopt;
                    current_pos++;
                    while (this->input[current_pos] >= '0' && this->input[current_pos] <= '9') {
                        current_pos++;
                    }
                }
                //Initialize token string.
                token_string = string_type(this->input.data() + next_token_starting_pos, current_pos - next_token_starting_pos);
                auto to_parse = std::string(token_string);
                if (is_integer) {
                    integer_type int_value;
                    try {
                        int_value = std::stoll(to_parse);
                    } catch (const std::invalid_argument& e) {
                        return std::nullopt;
                    } catch (const std::out_of_range& e) {
                        return std::nullopt;
                    }
                    return token(token_type::INTEGER, token_string, int_value);
                }
                float_type float_value;
                try {
                    float_value = std::stod(to_parse);
                } catch (const std::invalid_argument& e) {
                    return std::nullopt;
                } catch (const std::out_of_range& e) {
                    return std::nullopt;
                }
                return token(token_type::FLOAT, token_string, float_value);
        }
        current_pos++;
    }
    return std::nullopt;
}

jayson::tokenizer jayson::tokenize(std::string_view input) {
    return tokenizer(input);
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
    (void) key;
    return 0;
}

jayson::integer_type jayson::jayson_array::size() const {
    return 0;
}

std::vector<const jayson::jayson_element *> jayson::jayson_array::get_elements() const {
    return {};
}

const jayson::jayson_element *jayson::jayson_array::get_value_at(jayson::integer_type index) const {
    (void) index;
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
    (void) tokens;
    return {};
}

std::unique_ptr<jayson::jayson_element> jayson::parse_direct(std::string_view input) {
    return parse(tokenize(input));
}
