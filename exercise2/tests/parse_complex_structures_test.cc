#include <gtest/gtest.h>
#include <string>
#include <string_view>

#include "jayson.hpp"
#include "jayson_fixed.hpp"

#include "jayson_testhelper.hpp"

using namespace std::literals;

//------------------------------------------------------------------------------
// NESTED OBJECTS
//------------------------------------------------------------------------------

TEST(ParseComplexStructures, SimpleNestedObject) {
    test_parse_both(
        "{"
        "\"outer\": {"
        "\"inner\": \"value\""
        "}"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

            auto outer_obj = result->to_object();
            ASSERT_TRUE(outer_obj);
            EXPECT_EQ(outer_obj->size(), 1);

            auto inner_element = outer_obj->get_value_for("outer");
            ASSERT_TRUE(inner_element);
            EXPECT_EQ(inner_element->get_type(), jayson::jayson_types::OBJECT);

            auto inner_obj = inner_element->to_object();
            ASSERT_TRUE(inner_obj);
            EXPECT_EQ(inner_obj->size(), 1);

            auto value = inner_obj->get_value_for("inner");
            ASSERT_TRUE(value);
            EXPECT_EQ(value->get_type(), jayson::jayson_types::STRING);
            EXPECT_EQ(value->to_string()->get_string(), "value");
        });
}

TEST(ParseComplexStructures, MultiLevelNestedObjects) {
    test_parse_both(
        "{"
        "\"level1\": {"
        "\"level2\": {"
        "\"level3\": {"
        "\"data\": 42"
        "}"
        "}"
        "}"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            // Navigate through the nesting
            auto l1_obj = result->to_object();
            ASSERT_TRUE(l1_obj);

            auto l2_element = l1_obj->get_value_for("level1");
            ASSERT_TRUE(l2_element);
            EXPECT_EQ(l2_element->get_type(), jayson::jayson_types::OBJECT);

            auto l2_obj = l2_element->to_object();
            ASSERT_TRUE(l2_obj);

            auto l3_element = l2_obj->get_value_for("level2");
            ASSERT_TRUE(l3_element);
            EXPECT_EQ(l3_element->get_type(), jayson::jayson_types::OBJECT);

            auto l3_obj = l3_element->to_object();
            ASSERT_TRUE(l3_obj);

            auto l4_element = l3_obj->get_value_for("level3");
            ASSERT_TRUE(l4_element);
            EXPECT_EQ(l4_element->get_type(), jayson::jayson_types::OBJECT);

            auto l4_obj = l4_element->to_object();
            ASSERT_TRUE(l4_obj);

            auto data = l4_obj->get_value_for("data");
            ASSERT_TRUE(data);
            EXPECT_EQ(data->get_type(), jayson::jayson_types::INTEGER);
            EXPECT_EQ(data->to_integer()->get_integer(), 42);
        });
}

TEST(ParseComplexStructures, NestedObjectsWithMultipleKeys) {
    test_parse_both(
        "{"
        "\"config\": {"
        "\"database\": {"
        "\"host\": \"localhost\","
        "\"port\": 5432,"
        "\"ssl\": true"
        "},"
        "\"cache\": {"
        "\"enabled\": false,"
        "\"ttl\": null"
        "}"
        "},"
        "\"version\": \"1.0\""
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            auto root = result->to_object();
            ASSERT_TRUE(root);
            EXPECT_EQ(root->size(), 2);

            // Check version at root level
            auto version = root->get_value_for("version");
            ASSERT_TRUE(version);
            EXPECT_EQ(version->to_string()->get_string(), "1.0");

            // Check config object
            auto config_element = root->get_value_for("config");
            ASSERT_TRUE(config_element);
            auto config = config_element->to_object();
            ASSERT_TRUE(config);
            EXPECT_EQ(config->size(), 2);

            // Check database config
            auto db_element = config->get_value_for("database");
            ASSERT_TRUE(db_element);
            auto db = db_element->to_object();
            ASSERT_TRUE(db);
            EXPECT_EQ(db->size(), 3);

            EXPECT_EQ(db->get_value_for("host")->to_string()->get_string(), "localhost");
            EXPECT_EQ(db->get_value_for("port")->to_integer()->get_integer(), 5432);
            EXPECT_TRUE(db->get_value_for("ssl")->to_boolean()->get_boolean());

            // Check cache config
            auto cache_element = config->get_value_for("cache");
            ASSERT_TRUE(cache_element);
            auto cache = cache_element->to_object();
            ASSERT_TRUE(cache);
            EXPECT_EQ(cache->size(), 2);

            EXPECT_FALSE(cache->get_value_for("enabled")->to_boolean()->get_boolean());
            EXPECT_EQ(cache->get_value_for("ttl")->get_type(), jayson::jayson_types::NONE);
        });
}

//------------------------------------------------------------------------------
// NESTED ARRAYS
//------------------------------------------------------------------------------

TEST(ParseComplexStructures, SimpleNestedArray) {
    test_parse_both("[[1, 2], [3, 4]]", [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY);

        auto outer_arr = result->to_array();
        ASSERT_TRUE(outer_arr);
        EXPECT_EQ(outer_arr->size(), 2);

        // Check first inner array
        auto inner1 = outer_arr->get_value_at(0);
        ASSERT_TRUE(inner1);
        EXPECT_EQ(inner1->get_type(), jayson::jayson_types::ARRAY);

        auto inner1_arr = inner1->to_array();
        ASSERT_TRUE(inner1_arr);
        EXPECT_EQ(inner1_arr->size(), 2);
        EXPECT_EQ(inner1_arr->get_value_at(0)->to_integer()->get_integer(), 1);
        EXPECT_EQ(inner1_arr->get_value_at(1)->to_integer()->get_integer(), 2);

        // Check second inner array
        auto inner2 = outer_arr->get_value_at(1);
        ASSERT_TRUE(inner2);
        EXPECT_EQ(inner2->get_type(), jayson::jayson_types::ARRAY);

        auto inner2_arr = inner2->to_array();
        ASSERT_TRUE(inner2_arr);
        EXPECT_EQ(inner2_arr->size(), 2);
        EXPECT_EQ(inner2_arr->get_value_at(0)->to_integer()->get_integer(), 3);
        EXPECT_EQ(inner2_arr->get_value_at(1)->to_integer()->get_integer(), 4);
    });
}

TEST(ParseComplexStructures, DeepNestedArray) {
    test_parse_both("[[[[[42]]]]]", [](auto, auto result) {
        ASSERT_TRUE(result);

        // Navigate through nested arrays
        auto current = result->to_array();
        for (int i = 0; i < 4; ++i) {
            ASSERT_TRUE(current);
            EXPECT_EQ(current->size(), 1);

            auto next_element = current->get_value_at(0);
            ASSERT_TRUE(next_element);
            EXPECT_EQ(next_element->get_type(), jayson::jayson_types::ARRAY);

            current = next_element->to_array();
        }

        // Final level should contain the integer
        ASSERT_TRUE(current);
        EXPECT_EQ(current->size(), 1);

        auto final_value = current->get_value_at(0);
        ASSERT_TRUE(final_value);
        EXPECT_EQ(final_value->get_type(), jayson::jayson_types::INTEGER);
        EXPECT_EQ(final_value->to_integer()->get_integer(), 42);
    });
}

TEST(ParseComplexStructures, ArrayOfArraysWithDifferentTypes) {
    test_parse_both(
        "["
        "[\"strings\", \"here\"],"
        "[1, 2, 3],"
        "[true, false, null],"
        "[3.14, 2.718]"
        "]",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            auto outer = result->to_array();
            ASSERT_TRUE(outer);
            EXPECT_EQ(outer->size(), 4);

            // Array of strings
            auto strings_arr = outer->get_value_at(0)->to_array();
            ASSERT_TRUE(strings_arr);
            EXPECT_EQ(strings_arr->size(), 2);
            EXPECT_EQ(strings_arr->get_value_at(0)->to_string()->get_string(), "strings");
            EXPECT_EQ(strings_arr->get_value_at(1)->to_string()->get_string(), "here");

            // Array of integers
            auto ints_arr = outer->get_value_at(1)->to_array();
            ASSERT_TRUE(ints_arr);
            EXPECT_EQ(ints_arr->size(), 3);
            for (int i = 0; i < 3; ++i) {
                EXPECT_EQ(ints_arr->get_value_at(i)->to_integer()->get_integer(), i + 1);
            }

            // Array of booleans and null
            auto mixed_arr = outer->get_value_at(2)->to_array();
            ASSERT_TRUE(mixed_arr);
            EXPECT_EQ(mixed_arr->size(), 3);
            EXPECT_TRUE(mixed_arr->get_value_at(0)->to_boolean()->get_boolean());
            EXPECT_FALSE(mixed_arr->get_value_at(1)->to_boolean()->get_boolean());
            EXPECT_EQ(mixed_arr->get_value_at(2)->get_type(), jayson::jayson_types::NONE);

            // Array of floats
            auto floats_arr = outer->get_value_at(3)->to_array();
            ASSERT_TRUE(floats_arr);
            EXPECT_EQ(floats_arr->size(), 2);
            EXPECT_DOUBLE_EQ(floats_arr->get_value_at(0)->to_float()->get_float(), 3.14);
            EXPECT_DOUBLE_EQ(floats_arr->get_value_at(1)->to_float()->get_float(), 2.718);
        });
}

//------------------------------------------------------------------------------
// MIXED NESTING (OBJECTS AND ARRAYS)
//------------------------------------------------------------------------------

TEST(ParseComplexStructures, ObjectContainingArrays) {
    test_parse_both(
        "{"
        "\"numbers\": [1, 2, 3],"
        "\"words\": [\"hello\", \"world\"],"
        "\"nested\": [{"
        "\"inner\": \"value\""
        "}]"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            auto obj = result->to_object();
            ASSERT_TRUE(obj);
            EXPECT_EQ(obj->size(), 3);

            // Check numbers array
            auto numbers = obj->get_value_for("numbers")->to_array();
            ASSERT_TRUE(numbers);
            EXPECT_EQ(numbers->size(), 3);
            for (int i = 0; i < 3; ++i) {
                EXPECT_EQ(numbers->get_value_at(i)->to_integer()->get_integer(), i + 1);
            }

            // Check words array
            auto words = obj->get_value_for("words")->to_array();
            ASSERT_TRUE(words);
            EXPECT_EQ(words->size(), 2);
            EXPECT_EQ(words->get_value_at(0)->to_string()->get_string(), "hello");
            EXPECT_EQ(words->get_value_at(1)->to_string()->get_string(), "world");

            // Check nested array containing object
            auto nested = obj->get_value_for("nested")->to_array();
            ASSERT_TRUE(nested);
            EXPECT_EQ(nested->size(), 1);

            auto inner_obj = nested->get_value_at(0)->to_object();
            ASSERT_TRUE(inner_obj);
            EXPECT_EQ(inner_obj->size(), 1);
            EXPECT_EQ(inner_obj->get_value_for("inner")->to_string()->get_string(), "value");
        });
}

TEST(ParseComplexStructures, ArrayContainingObjects) {
    test_parse_both(
        "["
        "{\"name\": \"Alice\", \"age\": 30},"
        "{\"name\": \"Bob\", \"age\": 25},"
        "{\"name\": \"Charlie\", \"age\": 35}"
        "]",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            auto arr = result->to_array();
            ASSERT_TRUE(arr);
            EXPECT_EQ(arr->size(), 3);

            std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
            std::vector<int> ages          = {30, 25, 35};

            for (int i = 0; i < 3; ++i) {
                auto person = arr->get_value_at(i)->to_object();
                ASSERT_TRUE(person);
                EXPECT_EQ(person->size(), 2);

                EXPECT_EQ(person->get_value_for("name")->to_string()->get_string(), names[i]);
                EXPECT_EQ(person->get_value_for("age")->to_integer()->get_integer(), ages[i]);
            }
        });
}

TEST(ParseComplexStructures, DeepMixedNesting) {
    test_parse_both(
        "{"
        "\"data\": {"
        "\"users\": ["
        "{"
        "\"profile\": {"
        "\"contact\": {"
        "\"emails\": [\"work@example.com\", \"personal@example.com\"]"
        "}"
        "}"
        "}"
        "]"
        "}"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            // Navigate the deep structure
            auto root    = result->to_object();
            auto data    = root->get_value_for("data")->to_object();
            auto users   = data->get_value_for("users")->to_array();
            auto user    = users->get_value_at(0)->to_object();
            auto profile = user->get_value_for("profile")->to_object();
            auto contact = profile->get_value_for("contact")->to_object();
            auto emails  = contact->get_value_for("emails")->to_array();

            ASSERT_TRUE(emails);
            EXPECT_EQ(emails->size(), 2);
            EXPECT_EQ(emails->get_value_at(0)->to_string()->get_string(), "work@example.com");
            EXPECT_EQ(emails->get_value_at(1)->to_string()->get_string(), "personal@example.com");
        });
}

//------------------------------------------------------------------------------
// LARGE STRUCTURES
//------------------------------------------------------------------------------

TEST(ParseComplexStructures, WideObject) {
    // Create object with many keys
    std::string json = "{";
    for (int i = 0; i < 100; ++i) {
        if (i > 0) json += ",";
        json += "\"key" + std::to_string(i) + "\": " + std::to_string(i);
    }
    json += "}";

    test_parse_both(json, [](auto, auto result) {
        ASSERT_TRUE(result);

        auto obj = result->to_object();
        ASSERT_TRUE(obj);
        EXPECT_EQ(obj->size(), 100);

        // Check a few values
        for (int i = 0; i < 100; i += 10) {
            std::string key = "key" + std::to_string(i);
            auto value      = obj->get_value_for(key);
            ASSERT_TRUE(value);
            EXPECT_EQ(value->to_integer()->get_integer(), i);
        }
    });
}

TEST(ParseComplexStructures, LongArray) {
    // Create array with many elements
    std::string json = "[";
    for (int i = 0; i < 1000; ++i) {
        if (i > 0) json += ",";
        json += std::to_string(i);
    }
    json += "]";

    test_parse_both(json, [](auto, auto result) {
        ASSERT_TRUE(result);

        auto arr = result->to_array();
        ASSERT_TRUE(arr);
        EXPECT_EQ(arr->size(), 1000);

        // Check a few values
        for (int i = 0; i < 1000; i += 100) {
            auto value = arr->get_value_at(i);
            ASSERT_TRUE(value);
            EXPECT_EQ(value->to_integer()->get_integer(), i);
        }
    });
}

//------------------------------------------------------------------------------
// STRESS TESTS
//------------------------------------------------------------------------------

TEST(ParseComplexStructures, VeryDeepNesting) {
    // Create deeply nested structure
    std::string json;
    int depth = 50;

    // Create opening braces and keys
    for (int i = 0; i < depth; ++i) { json += "{\"level" + std::to_string(i) + "\":"; }

    // Add final value
    json += "42";

    // Add closing braces
    for (int i = 0; i < depth; ++i) { json += "}"; }

    test_parse_both(json, [=](auto, auto result) {
        ASSERT_TRUE(result);

        // Navigate to the deep value
        auto current = result->to_object();
        for (int i = 0; i < depth - 1; ++i) {
            ASSERT_TRUE(current);
            std::string key = "level" + std::to_string(i);
            auto next       = current->get_value_for(key);
            ASSERT_TRUE(next);
            current = next->to_object();
        }

        // Check final value
        std::string final_key = "level" + std::to_string(depth - 1);
        auto final_value      = current->get_value_for(final_key);
        ASSERT_TRUE(final_value);
        EXPECT_EQ(final_value->to_integer()->get_integer(), 42);
    });
}

//------------------------------------------------------------------------------
// ERROR CASES FOR COMPLEX STRUCTURES
//------------------------------------------------------------------------------

TEST(ParseComplexStructures, InvalidNestedStructures) {
    // Unclosed nested object
    check_parse_fails("{\"outer\": {\"inner\": \"value\"}");

    // Unclosed nested array
    check_parse_fails("[1, [2, 3]");

    // Mismatched brackets in nested structure
    check_parse_fails("{\"array\": [1, 2}");
    check_parse_fails("{\"object\": {\"key\": \"value\"]}");

    // Missing comma in nested structure
    check_parse_fails("{\"a\": {\"x\": 1 \"y\": 2}}");
    check_parse_fails("[1, [2 3]]");

    // Invalid key in nested object
    check_parse_fails("{\"outer\": {42: \"value\"}}");

    // Trailing comma in nested structure
    check_parse_fails("{\"array\": [1, 2,]}");
    check_parse_fails("{\"object\": {\"key\": \"value\",}}");
}

//------------------------------------------------------------------------------
// WHITESPACE AND COMMENTS IN COMPLEX STRUCTURES
//------------------------------------------------------------------------------

TEST(ParseComplexStructures, ComplexStructureWithComments) {
    test_parse_both(
        "{\n"
        "  // Main configuration\n"
        "  \"config\": {\n"
        "    \"database\": { // Database settings\n"
        "      \"host\": \"localhost\", // Server host\n"
        "      \"port\": 5432 // Server port\n"
        "    },\n"
        "    \"features\": [ // Enabled features\n"
        "      \"auth\", // Authentication\n"
        "      \"cache\", // Caching\n"
        "      \"logging\" // Logging\n"
        "    ]\n"
        "  }\n"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            auto root     = result->to_object();
            auto config   = root->get_value_for("config")->to_object();
            auto database = config->get_value_for("database")->to_object();
            auto features = config->get_value_for("features")->to_array();

            EXPECT_EQ(database->get_value_for("host")->to_string()->get_string(), "localhost");
            EXPECT_EQ(database->get_value_for("port")->to_integer()->get_integer(), 5432);
            EXPECT_EQ(features->size(), 3);
            EXPECT_EQ(features->get_value_at(0)->to_string()->get_string(), "auth");
            EXPECT_EQ(features->get_value_at(1)->to_string()->get_string(), "cache");
            EXPECT_EQ(features->get_value_at(2)->to_string()->get_string(), "logging");
        });
}
