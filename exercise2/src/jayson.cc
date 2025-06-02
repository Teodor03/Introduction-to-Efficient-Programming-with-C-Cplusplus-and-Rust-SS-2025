#include "../include/jayson.hpp"

#include <ranges>
#include <variant>

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
                if (this->input[current_pos] < '0' || this->input[current_pos] > '9')
                    return std::nullopt;
                current_pos++;
                while (this->input[current_pos] >= '0' && this->input[current_pos] <= '9') {
                    current_pos++;
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

jayson::jayson_types jayson::jayson_object::get_type() const {
    return jayson_types::OBJECT;
}

jayson::integer_type jayson::jayson_object::size() const {
    return static_cast<integer_type>(map.size());
}

std::vector<jayson::string_type> jayson::jayson_object::get_keys() const {
    std::vector<string_type> result;
    for (const auto key: map | std::views::keys) {
        result.push_back(key);
    }
    return result;
}

std::vector<const jayson::jayson_element *> jayson::jayson_object::get_values() const {
    std::vector<const jayson_element *> result;
    for (auto&& val : this->map) {
        result.push_back(val.second.get());
    }
    return result;
}

const jayson::jayson_element *jayson::jayson_object::get_value_for(const string_type &key) const {
    const auto it = map.find(key);
    return it == map.end() ? nullptr : it->second.get();
}

jayson::jayson_types jayson::jayson_array::get_type() const {
    return jayson_types::ARRAY;
}

jayson::integer_type jayson::jayson_array::size() const {
    return static_cast<integer_type>(this->array.size());
}

std::vector<const jayson::jayson_element *> jayson::jayson_array::get_elements() const {
    std::vector<const jayson::jayson_element *> result;
    for (const auto& i : this->array) {
        result.push_back(i.get());
    }
    return result;
}

const jayson::jayson_element *jayson::jayson_array::get_value_at(integer_type index) const {
    if (index < 0 || index >= this->size())
        return nullptr;
    return this->array[index].get();
}

jayson::jayson_string::jayson_string(const string_type& value) : string(value) {
}

jayson::jayson_types jayson::jayson_string::get_type() const {
    return jayson_types::STRING;
}

jayson::string_type jayson::jayson_string::get_string() const {
    return this->string;
}

jayson::jayson_integer::jayson_integer(const integer_type &value) : integer(value) {
}

jayson::jayson_types jayson::jayson_integer::get_type() const {
    return jayson_types::INTEGER;
}

jayson::integer_type jayson::jayson_integer::get_integer() const {
    return this->integer;
}

jayson::jayson_float::jayson_float(const float_type &value) : floating(value) {
}

jayson::jayson_types jayson::jayson_float::get_type() const {
    return jayson_types::FLOAT;
}

jayson::float_type jayson::jayson_float::get_float() const {
    return this->floating;
}

jayson::jayson_boolean::jayson_boolean(const bool &value) : boolean(value) {
}

jayson::jayson_types jayson::jayson_boolean::get_type() const {
    return jayson_types::BOOLEAN;
}

bool jayson::jayson_boolean::get_boolean() const {
    return this->boolean;
}

jayson::jayson_types jayson::jayson_none::get_type() const {
    return jayson_types::NONE;
}

jayson::jayson_element::jayson_element(std::unique_ptr<jayson_element_base> element): element(std::move(element)) {
}

jayson::jayson_types jayson::jayson_element::get_type() const {
    return this->element->get_type();
}

const jayson::jayson_object *jayson::jayson_element::to_object() const {
    return dynamic_cast<const jayson_object *>(this->element.get());
}

const jayson::jayson_array *jayson::jayson_element::to_array() const {
    return dynamic_cast<const jayson_array *>(this->element.get());
}

const jayson::jayson_string *jayson::jayson_element::to_string() const {
    return dynamic_cast<const jayson_string *>(this->element.get());
}

const jayson::jayson_integer *jayson::jayson_element::to_integer() const {
    return dynamic_cast<const jayson_integer *>(this->element.get());
}

const jayson::jayson_float *jayson::jayson_element::to_float() const {
    return dynamic_cast<const jayson_float *>(this->element.get());
}

const jayson::jayson_boolean *jayson::jayson_element::to_boolean() const {
    return dynamic_cast<const jayson_boolean *>(this->element.get());
}

const jayson::jayson_none *jayson::jayson_element::to_none() const {
    return dynamic_cast<const jayson_none *>(this->element.get());
}

std::optional<jayson::token> get_next_non_comment_token(jayson::tokenizer& tokens) {
    while (true) {
        auto t = tokens.get_next_token();
        if (!t.has_value())
            return t;
        if (t.value().get_type() != jayson::token_type::COMMENT)
            return t;
    }
}

std::optional<jayson::token> peek_next_non_comment_token(jayson::tokenizer& tokens) {
    while (true) {
        auto t = tokens.peek_next_token();
        if (!t.has_value())
            return t;
        if (t.value().get_type() != jayson::token_type::COMMENT)
            return t;
        t = tokens.get_next_token();
    }
}

std::unique_ptr<jayson::jayson_element> parse_jayson_element(jayson::tokenizer& tokens);

std::unique_ptr<jayson::jayson_element> parse_jayson_object(jayson::tokenizer& tokens) {
    auto object = std::make_unique<jayson::jayson_object>();
    auto t = get_next_non_comment_token(tokens);
    if (!t.has_value())
        return nullptr;
    if (t.value().get_type() == jayson::token_type::OBJECT_END)
        return std::make_unique<jayson::jayson_element>(std::move(object));
    while (true) {
        if (t.value().get_type() != jayson::token_type::STRING)
            return nullptr;
        auto key = t.value().get_string().value();
        t = get_next_non_comment_token(tokens);
        if (!t.has_value())
            return nullptr;
        if (t.value().get_type() != jayson::token_type::COLON)
            return nullptr;
        auto key_pair = parse_jayson_element(tokens);
        if (key_pair == nullptr)
            return nullptr;
        object->map[key] = std::move(key_pair);
        t = get_next_non_comment_token(tokens);
        if (!t.has_value())
            return nullptr;
        if (t.value().get_type() == jayson::token_type::OBJECT_END)
            return std::make_unique<jayson::jayson_element>(std::move(object));
        if (t.value().get_type() == jayson::token_type::COMMA) {
            t = get_next_non_comment_token(tokens);
            if (!t.has_value())
                return nullptr;
        } else {
            return nullptr;
        }
    }
}

std::unique_ptr<jayson::jayson_element> parse_jayson_array(jayson::tokenizer &tokens) {
    auto array = std::make_unique<jayson::jayson_array>();
    auto t = peek_next_non_comment_token(tokens);
    if (!t.has_value())
        return nullptr;
    if (t.value().get_type() == jayson::token_type::ARRAY_END) {
        t = get_next_non_comment_token(tokens);
        return std::make_unique<jayson::jayson_element>(std::move(array));
    }
    while (true) {
        auto array_element = parse_jayson_element(tokens);
        if (array_element == nullptr)
            return nullptr;
        array->array.push_back(std::move(array_element));
        t = peek_next_non_comment_token(tokens);
        if (!t.has_value())
            return nullptr;
        if (t.value().get_type() == jayson::token_type::ARRAY_END) {
            t = get_next_non_comment_token(tokens);
            return std::make_unique<jayson::jayson_element>(std::move(array));
        }
        if (t.value().get_type() == jayson::token_type::COMMA) {
            t = get_next_non_comment_token(tokens);
        } else {
            return nullptr;
        }
    }
}

std::unique_ptr<jayson::jayson_element> parse_jayson_element(jayson::tokenizer& tokens) {
    auto t = get_next_non_comment_token(tokens);
    if (!t.has_value())
        return nullptr;
    switch (t.value().get_type()) {
        case jayson::token_type::OBJECT_BEGIN:
            return parse_jayson_object(tokens);
        case jayson::token_type::ARRAY_BEGIN:
            return parse_jayson_array(tokens);
        case jayson::token_type::NONE:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_none>());
        case jayson::token_type::STRING:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_string>(t.value().get_string().value()));
        case jayson::token_type::INTEGER:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_integer>(t.value().get_integer().value()));
        case jayson::token_type::FLOAT:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_float>(t.value().get_float().value()));
        case jayson::token_type::BOOLEAN:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_boolean>(t.value().get_boolean().value()));
        default:
            return nullptr;
    }
}

std::unique_ptr<jayson::jayson_element> jayson::parse(tokenizer tokens) {
    auto result = parse_jayson_element(tokens);
    if (peek_next_non_comment_token(tokens).has_value())
        return nullptr;
    return result;
}

std::unique_ptr<jayson::jayson_element> parse_jayson_primitive(const jayson::token& t) {
    switch (t.get_type()) {
        case jayson::token_type::STRING:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_string>(*t.get_string()));
        case jayson::token_type::INTEGER:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_integer>(*t.get_integer()));
        case jayson::token_type::FLOAT:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_float>(*t.get_float()));
        case jayson::token_type::BOOLEAN:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_boolean>(*t.get_boolean()));
        case jayson::token_type::NONE:
            return std::make_unique<jayson::jayson_element>(std::make_unique<jayson::jayson_none>());
        default:
            return nullptr;
    }
}

std::unique_ptr<jayson::jayson_element> jayson::parse_direct(std::string_view input) {
    tokenizer tokens = tokenize(input);
    std::vector<std::variant<jayson_object, jayson_array>> container_stack;
    std::vector<std::optional<string_type>> key_stack;
    std::vector<token_type> context_stack;
    std::unique_ptr<jayson_element> primitive;

    auto t = get_next_non_comment_token(tokens);
    if (!t.has_value())
        return nullptr;

    while (true) {
        if (t->get_type() == token_type::OBJECT_BEGIN) {
            auto next = peek_next_non_comment_token(tokens);
            if (next.has_value() && next->get_type() == token_type::OBJECT_END) {
                get_next_non_comment_token(tokens);
                primitive = std::make_unique<jayson_element>(std::make_unique<jayson_object>());
            } else {
                container_stack.emplace_back(jayson_object{});
                context_stack.push_back(token_type::OBJECT_BEGIN);
                key_stack.emplace_back(std::nullopt);
                t = get_next_non_comment_token(tokens);
                continue;
            }
        } else if (t->get_type() == token_type::ARRAY_BEGIN) {
            auto next = peek_next_non_comment_token(tokens);
            if (next.has_value() && next->get_type() == token_type::ARRAY_END) {
                get_next_non_comment_token(tokens);
                primitive = std::make_unique<jayson_element>(std::make_unique<jayson_array>());
            } else {
                container_stack.emplace_back(jayson_array{});
                context_stack.push_back(token_type::ARRAY_BEGIN);
                key_stack.emplace_back(std::nullopt);
                t = get_next_non_comment_token(tokens);
                continue;
            }
        } else {
            primitive = parse_jayson_primitive(*t);
            if (!primitive)
                return nullptr;
        }

        while (!container_stack.empty()) {
            auto& ctx = context_stack.back();

            if (ctx == token_type::ARRAY_BEGIN) {
                auto& arr = std::get<jayson_array>(container_stack.back());
                arr.array.push_back(std::move(primitive));

                auto next = peek_next_non_comment_token(tokens);
                if (!next.has_value())
                    return nullptr;

                if (next->get_type() == token_type::COMMA) {
                    get_next_non_comment_token(tokens);
                    t = get_next_non_comment_token(tokens);
                    if (!t.has_value()) return nullptr;
                    break;
                }
                if (next->get_type() == token_type::ARRAY_END) {
                    get_next_non_comment_token(tokens); // consume ']'
                    auto done = std::make_unique<jayson_element>(std::make_unique<jayson_array>(std::move(arr)));
                    container_stack.pop_back();
                    context_stack.pop_back();
                    key_stack.pop_back();
                    primitive = std::move(done);
                } else {
                    return nullptr;
                }
            } else if (ctx == token_type::OBJECT_BEGIN) {
                auto& key = key_stack.back();
                if (!key.has_value()) {
                    if (t->get_type() != token_type::STRING)
                        return nullptr;
                    key = *t->get_string();

                    t = get_next_non_comment_token(tokens);
                    if (!t.has_value() || t->get_type() != token_type::COLON)
                        return nullptr;

                    t = get_next_non_comment_token(tokens);
                    if (!t.has_value())
                        return nullptr;
                    break;
                }

                auto& obj = std::get<jayson_object>(container_stack.back());
                obj.map[*key] = std::move(primitive);
                key.reset();

                auto next = peek_next_non_comment_token(tokens);
                if (!next.has_value())
                    return nullptr;

                if (next->get_type() == token_type::COMMA) {
                    get_next_non_comment_token(tokens);
                    t = get_next_non_comment_token(tokens);
                    if (!t.has_value())
                        return nullptr;
                    break;
                }
                if (next->get_type() == token_type::OBJECT_END) {
                    get_next_non_comment_token(tokens);
                    auto done = std::make_unique<jayson_element>(std::make_unique<jayson_object>(std::move(obj)));
                    container_stack.pop_back();
                    context_stack.pop_back();
                    key_stack.pop_back();
                    primitive = std::move(done);
                } else {
                    return nullptr;
                }
            }
        }
        if (container_stack.empty()) {
            auto trailing = peek_next_non_comment_token(tokens);
            if (trailing.has_value())
                return nullptr;
            return primitive;
        }
    }
}
