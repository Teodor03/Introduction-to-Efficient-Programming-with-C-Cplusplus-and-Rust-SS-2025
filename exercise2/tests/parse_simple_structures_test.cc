#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>

#include "jayson.hpp"
#include "jayson_fixed.hpp"

#include "jayson_testhelper.hpp"

using namespace std::literals;

//------------------------------------------------------------------------------
// EMPTY STRUCTURES
//------------------------------------------------------------------------------

TEST(ParseSimpleStructures, EmptyObject) {
    test_parse_both("{}", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

        auto obj = result->to_object();
        ASSERT_TRUE(obj);
        EXPECT_EQ(obj->size(), 0);
        EXPECT_TRUE(obj->get_keys().empty());
        EXPECT_TRUE(obj->get_values().empty());
    });
}

TEST(ParseSimpleStructures, EmptyArray) {
    test_parse_both("[]", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY);

        auto arr = result->to_array();
        ASSERT_TRUE(arr);
        EXPECT_EQ(arr->size(), 0);
        EXPECT_TRUE(arr->get_elements().empty());
    });
}

TEST(ParseSimpleStructures, EmptyStructuresWithWhitespace) {
    // Empty object with whitespace
    test_parse_both("{ }", [](auto, auto obj_result) {
        ASSERT_TRUE(obj_result);
        EXPECT_EQ(obj_result->get_type(), jayson::jayson_types::OBJECT);
        EXPECT_EQ(obj_result->to_object()->size(), 0);
    });

    // Empty array with whitespace
    test_parse_both("[ ]", [](auto, auto arr_result) {
        ASSERT_TRUE(arr_result);
        EXPECT_EQ(arr_result->get_type(), jayson::jayson_types::ARRAY);
        EXPECT_EQ(arr_result->to_array()->size(), 0);
    });

    // With various whitespace types
    test_parse_both("{\t\n\r }", [](auto, auto obj_ws) {
        ASSERT_TRUE(obj_ws);
        EXPECT_EQ(obj_ws->get_type(), jayson::jayson_types::OBJECT);
    });

    test_parse_both("[\t\n\r ]", [](auto, auto arr_ws) {
        ASSERT_TRUE(arr_ws);
        EXPECT_EQ(arr_ws->get_type(), jayson::jayson_types::ARRAY);
    });
}

TEST(ParseSimpleStructures, EmptyStructuresWithComments) {
    // Empty object with comments
    test_parse_both("{ // empty object\n}", [](auto, auto obj_result) {
        ASSERT_TRUE(obj_result);
        EXPECT_EQ(obj_result->get_type(), jayson::jayson_types::OBJECT);
        EXPECT_EQ(obj_result->to_object()->size(), 0);
    });

    // Empty array with comments
    test_parse_both("[ // empty array\n]", [](auto, auto arr_result) {
        ASSERT_TRUE(arr_result);
        EXPECT_EQ(arr_result->get_type(), jayson::jayson_types::ARRAY);
        EXPECT_EQ(arr_result->to_array()->size(), 0);
    });
}

//------------------------------------------------------------------------------
// SIMPLE OBJECTS
//------------------------------------------------------------------------------

TEST(ParseSimpleStructures, SingleKeyObject) {
    test_parse_both("{\"key\": \"value\"}", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

        auto obj = result->to_object();
        ASSERT_TRUE(obj);
        EXPECT_EQ(obj->size(), 1);

        auto keys = obj->get_keys();
        ASSERT_EQ(keys.size(), 1);
        EXPECT_EQ(keys[0], "key");

        auto value = obj->get_value_for("key");
        ASSERT_TRUE(value);
        EXPECT_EQ(value->get_type(), jayson::jayson_types::STRING);
        EXPECT_EQ(value->to_string()->get_string(), "value");
    });
}

TEST(ParseSimpleStructures, MultipleKeyObject) {
    test_parse_both("{\"a\": 1, \"b\": 2, \"c\": 3}", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

        auto obj = result->to_object();
        ASSERT_TRUE(obj);
        EXPECT_EQ(obj->size(), 3);

        auto keys = obj->get_keys();
        EXPECT_EQ(keys.size(), 3);

        // Check that all keys are present (order may vary)
        std::vector<std::string> expected_keys = {"a", "b", "c"};
        for (const auto &key : expected_keys) {
            bool found = false;
            for (const auto &actual_key : keys) {
                if (actual_key == key) {
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found) << "Key '" << key << "' not found";
        }

        // Check values
        auto value_a = obj->get_value_for("a");
        ASSERT_TRUE(value_a);
        EXPECT_EQ(value_a->get_type(), jayson::jayson_types::INTEGER);
        EXPECT_EQ(value_a->to_integer()->get_integer(), 1);

        auto value_b = obj->get_value_for("b");
        ASSERT_TRUE(value_b);
        EXPECT_EQ(value_b->get_type(), jayson::jayson_types::INTEGER);
        EXPECT_EQ(value_b->to_integer()->get_integer(), 2);

        auto value_c = obj->get_value_for("c");
        ASSERT_TRUE(value_c);
        EXPECT_EQ(value_c->get_type(), jayson::jayson_types::INTEGER);
        EXPECT_EQ(value_c->to_integer()->get_integer(), 3);
    });
}

TEST(ParseSimpleStructures, ObjectWithVariousValueTypes) {
    test_parse_both(
        "{"
        "\"str\": \"hello\", "
        "\"num\": 42, "
        "\"flt\": 3.14, "
        "\"bool\": true, "
        "\"null_val\": null"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

            auto obj = result->to_object();
            ASSERT_TRUE(obj);
            EXPECT_EQ(obj->size(), 5);

            // Check string value
            auto str_val = obj->get_value_for("str");
            ASSERT_TRUE(str_val);
            EXPECT_EQ(str_val->get_type(), jayson::jayson_types::STRING);
            EXPECT_EQ(str_val->to_string()->get_string(), "hello");

            // Check integer value
            auto num_val = obj->get_value_for("num");
            ASSERT_TRUE(num_val);
            EXPECT_EQ(num_val->get_type(), jayson::jayson_types::INTEGER);
            EXPECT_EQ(num_val->to_integer()->get_integer(), 42);

            // Check float value
            auto flt_val = obj->get_value_for("flt");
            ASSERT_TRUE(flt_val);
            EXPECT_EQ(flt_val->get_type(), jayson::jayson_types::FLOAT);
            EXPECT_DOUBLE_EQ(flt_val->to_float()->get_float(), 3.14);

            // Check boolean value
            auto bool_val = obj->get_value_for("bool");
            ASSERT_TRUE(bool_val);
            EXPECT_EQ(bool_val->get_type(), jayson::jayson_types::BOOLEAN);
            EXPECT_TRUE(bool_val->to_boolean()->get_boolean());

            // Check null value
            auto null_val = obj->get_value_for("null_val");
            ASSERT_TRUE(null_val);
            EXPECT_EQ(null_val->get_type(), jayson::jayson_types::NONE);
        });
}

//------------------------------------------------------------------------------
// SIMPLE ARRAYS
//------------------------------------------------------------------------------

TEST(ParseSimpleStructures, SingleElementArray) {
    test_parse_both("[\"value\"]", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY);

        auto arr = result->to_array();
        ASSERT_TRUE(arr);
        EXPECT_EQ(arr->size(), 1);

        auto elements = arr->get_elements();
        ASSERT_EQ(elements.size(), 1);
        EXPECT_EQ(elements[0]->get_type(), jayson::jayson_types::STRING);
        EXPECT_EQ(elements[0]->to_string()->get_string(), "value");

        // Test get_value_at
        auto value = arr->get_value_at(0);
        ASSERT_TRUE(value);
        EXPECT_EQ(value->get_type(), jayson::jayson_types::STRING);
        EXPECT_EQ(value->to_string()->get_string(), "value");

        // Out of bounds should return nullptr
        EXPECT_FALSE(arr->get_value_at(-1));
        EXPECT_FALSE(arr->get_value_at(1));
    });
}

TEST(ParseSimpleStructures, MultipleElementArray) {
    test_parse_both("[1, 2, 3, 4, 5]", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY);

        auto arr = result->to_array();
        ASSERT_TRUE(arr);
        EXPECT_EQ(arr->size(), 5);

        auto elements = arr->get_elements();
        ASSERT_EQ(elements.size(), 5);

        // Check all elements
        for (int i = 0; i < 5; ++i) {
            EXPECT_EQ(elements[i]->get_type(), jayson::jayson_types::INTEGER);
            EXPECT_EQ(elements[i]->to_integer()->get_integer(), i + 1);

            // Also test get_value_at
            auto value = arr->get_value_at(i);
            ASSERT_TRUE(value);
            EXPECT_EQ(value->get_type(), jayson::jayson_types::INTEGER);
            EXPECT_EQ(value->to_integer()->get_integer(), i + 1);
        }
    });
}

TEST(ParseSimpleStructures, ArrayWithVariousValueTypes) {
    test_parse_both("[\"hello\", 42, 3.14, true, null]", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY);

        auto arr = result->to_array();
        ASSERT_TRUE(arr);
        EXPECT_EQ(arr->size(), 5);

        auto elements = arr->get_elements();
        ASSERT_EQ(elements.size(), 5);

        // Check string element
        EXPECT_EQ(elements[0]->get_type(), jayson::jayson_types::STRING);
        EXPECT_EQ(elements[0]->to_string()->get_string(), "hello");

        // Check integer element
        EXPECT_EQ(elements[1]->get_type(), jayson::jayson_types::INTEGER);
        EXPECT_EQ(elements[1]->to_integer()->get_integer(), 42);

        // Check float element
        EXPECT_EQ(elements[2]->get_type(), jayson::jayson_types::FLOAT);
        EXPECT_DOUBLE_EQ(elements[2]->to_float()->get_float(), 3.14);

        // Check boolean element
        EXPECT_EQ(elements[3]->get_type(), jayson::jayson_types::BOOLEAN);
        EXPECT_TRUE(elements[3]->to_boolean()->get_boolean());

        // Check null element
        EXPECT_EQ(elements[4]->get_type(), jayson::jayson_types::NONE);
    });
}

//------------------------------------------------------------------------------
// WHITESPACE AND FORMATTING
//------------------------------------------------------------------------------

TEST(ParseSimpleStructures, ObjectWithWhitespace) {
    test_parse_both(
        "{\n"
        "  \"key1\" : \"value1\" ,\n"
        "  \"key2\" : 42\n"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

            auto obj = result->to_object();
            ASSERT_TRUE(obj);
            EXPECT_EQ(obj->size(), 2);

            auto value1 = obj->get_value_for("key1");
            ASSERT_TRUE(value1);
            EXPECT_EQ(value1->to_string()->get_string(), "value1");

            auto value2 = obj->get_value_for("key2");
            ASSERT_TRUE(value2);
            EXPECT_EQ(value2->to_integer()->get_integer(), 42);
        });
}

TEST(ParseSimpleStructures, ArrayWithWhitespace) {
    test_parse_both(
        "[\n"
        "  1 ,\n"
        "  2 ,\n"
        "  3\n"
        "]",
        [](auto, auto result) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY);

            auto arr = result->to_array();
            ASSERT_TRUE(arr);
            EXPECT_EQ(arr->size(), 3);

            for (int i = 0; i < 3; ++i) {
                auto value = arr->get_value_at(i);
                ASSERT_TRUE(value);
                EXPECT_EQ(value->to_integer()->get_integer(), i + 1);
            }
        });
}

TEST(ParseSimpleStructures, StructuresWithComments) {
    // Object with comments
    test_parse_both(
        "{\n"
        "  // First key\n"
        "  \"key1\": \"value1\", // String value\n"
        "  \"key2\": 42 // Integer value\n"
        "}",
        [](auto, auto obj_result) {
            ASSERT_TRUE(obj_result);
            EXPECT_EQ(obj_result->get_type(), jayson::jayson_types::OBJECT);
            EXPECT_EQ(obj_result->to_object()->size(), 2);
        });

    // Array with comments
    test_parse_both(
        "[\n"
        "  // First element\n"
        "  1, // Integer\n"
        "  2, // Another integer\n"
        "  3  // Last integer\n"
        "]",
        [](auto, auto arr_result) {
            ASSERT_TRUE(arr_result);
            EXPECT_EQ(arr_result->get_type(), jayson::jayson_types::ARRAY);
            EXPECT_EQ(arr_result->to_array()->size(), 3);
        });
}

//------------------------------------------------------------------------------
// ERROR CASES
//------------------------------------------------------------------------------

TEST(ParseSimpleStructures, InvalidObjects) {
    // Missing closing brace
    check_parse_fails("{\"key\": \"value\"");

    // Missing opening brace
    check_parse_fails("\"key\": \"value\"}");

    // Missing colon
    check_parse_fails("{\"key\" \"value\"}");

    // Missing value
    check_parse_fails("{\"key\":}");

    // Missing key
    check_parse_fails("{: \"value\"}");

    // Missing comma between pairs
    check_parse_fails("{\"key1\": \"value1\" \"key2\": \"value2\"}");

    // Trailing comma (should fail)
    check_parse_fails("{\"key\": \"value\",}");

    // Non-string key
    check_parse_fails("{42: \"value\"}");
    check_parse_fails("{true: \"value\"}");
}

TEST(ParseSimpleStructures, InvalidArrays) {
    // Missing closing bracket
    check_parse_fails("[1, 2, 3");

    // Missing opening bracket
    check_parse_fails("1, 2, 3]");

    // Missing comma between elements
    check_parse_fails("[1 2 3]");

    // Trailing comma (should fail)
    check_parse_fails("[1, 2, 3,]");

    // Empty element
    check_parse_fails("[1, , 3]");
}

TEST(ParseSimpleStructures, MismatchedBrackets) {
    // Object/array mismatch
    check_parse_fails("{]");
    check_parse_fails("[}");
    check_parse_fails("{\"key\": \"value\"]");
    check_parse_fails("[\"value\"}");
}

//------------------------------------------------------------------------------
// API CONSISTENCY TESTS
//------------------------------------------------------------------------------

TEST(ParseSimpleStructures, ObjectAPIConsistency) {
    test_parse_both("{\"a\": 1, \"b\": 2}", [](auto, auto result) {
        ASSERT_TRUE(result);

        auto obj = result->to_object();
        ASSERT_TRUE(obj);

        // get_keys() and get_values() should have same size
        auto keys   = obj->get_keys();
        auto values = obj->get_values();
        EXPECT_EQ(keys.size(), values.size());
        EXPECT_EQ(keys.size(), obj->size());

        // get_value_for(get_keys()[i]) == get_values()[i]
        for (size_t i = 0; i < keys.size(); ++i) {
            auto value_by_key   = obj->get_value_for(keys[i]);
            auto value_by_index = values[i];
            EXPECT_EQ(value_by_key, value_by_index);
        }

        // Non-existent key should return nullptr
        EXPECT_FALSE(obj->get_value_for("nonexistent"));
    });
}

TEST(ParseSimpleStructures, ArrayAPIConsistency) {
    test_parse_both("[1, 2, 3]", [](auto, auto result) {
        ASSERT_TRUE(result);

        auto arr = result->to_array();
        ASSERT_TRUE(arr);

        // get_elements() should have same size as size()
        auto elements = arr->get_elements();
        EXPECT_EQ(elements.size(), arr->size());

        // get_value_at(i) should match get_elements()[i]
        for (size_t i = 0; i < elements.size(); ++i) {
            auto value_by_index  = arr->get_value_at(i);
            auto value_by_vector = elements[i];
            EXPECT_EQ(value_by_index, value_by_vector);
        }

        // Out of bounds access should return nullptr
        EXPECT_FALSE(arr->get_value_at(-1));
        EXPECT_FALSE(arr->get_value_at(arr->size()));
        EXPECT_FALSE(arr->get_value_at(1000));
    });
}
