#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>

#include "jayson.hpp"
#include "jayson_fixed.hpp"
#include "jayson_print.hpp"

#include "jayson_testhelper.hpp"

using namespace std::literals;

//------------------------------------------------------------------------------
// VALID COMPLETE DOCUMENTS
//------------------------------------------------------------------------------

TEST(ParseCompleteDocuments, SimpleDocuments) {
    // Simple value documents
    check_string_value("\"hello world\"", "hello world");
    check_integer_value("42", 42);
    check_boolean_value("true", true);
    check_null_value("null");
}

TEST(ParseCompleteDocuments, ObjectDocuments) {
    test_parse_both(
        "{\n"
        "  \"name\": \"John Doe\",\n"
        "  \"age\": 30,\n"
        "  \"active\": true,\n"
        "  \"balance\": 1234.56,\n"
        "  \"address\": null\n"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

            auto obj = result->to_object();
            ASSERT_TRUE(obj);
            EXPECT_EQ(obj->size(), 5);

            EXPECT_EQ(obj->get_value_for("name")->to_string()->get_string(), "John Doe");
            EXPECT_EQ(obj->get_value_for("age")->to_integer()->get_integer(), 30);
            EXPECT_TRUE(obj->get_value_for("active")->to_boolean()->get_boolean());
            EXPECT_DOUBLE_EQ(obj->get_value_for("balance")->to_float()->get_float(), 1234.56);
            EXPECT_EQ(obj->get_value_for("address")->get_type(), jayson::jayson_types::NONE);
        });
}

TEST(ParseCompleteDocuments, ArrayDocuments) {
    test_parse_both(
        "[\n"
        "  \"apple\",\n"
        "  \"banana\",\n"
        "  \"cherry\",\n"
        "  {\n"
        "    \"name\": \"orange\",\n"
        "    \"color\": \"orange\"\n"
        "  },\n"
        "  false,"
        "  1,"
        "  2.3,"
        "  null"
        "]",
        [](auto, auto result) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result->get_type(), jayson::jayson_types::ARRAY);

            auto arr = result->to_array();
            ASSERT_TRUE(arr);
            EXPECT_EQ(arr->size(), 8);

            ASSERT_TRUE(arr->get_value_at(0));
            ASSERT_TRUE(arr->get_value_at(0)->to_string());
            EXPECT_EQ(arr->get_value_at(0)->to_string()->get_string(), "apple");
            ASSERT_TRUE(arr->get_value_at(1));
            ASSERT_TRUE(arr->get_value_at(1)->to_string());
            EXPECT_EQ(arr->get_value_at(1)->to_string()->get_string(), "banana");
            ASSERT_TRUE(arr->get_value_at(2));
            ASSERT_TRUE(arr->get_value_at(2)->to_string());
            EXPECT_EQ(arr->get_value_at(2)->to_string()->get_string(), "cherry");

            auto fruit_obj = arr->get_value_at(3)->to_object();
            ASSERT_TRUE(fruit_obj);
            EXPECT_EQ(fruit_obj->get_value_for("name")->to_string()->get_string(), "orange");
            EXPECT_EQ(fruit_obj->get_value_for("color")->to_string()->get_string(), "orange");

            ASSERT_TRUE(arr->get_value_at(4));
            ASSERT_TRUE(arr->get_value_at(4)->to_boolean());
            EXPECT_EQ(arr->get_value_at(4)->to_boolean()->get_boolean(), false);
            ASSERT_TRUE(arr->get_value_at(5));
            ASSERT_TRUE(arr->get_value_at(5)->to_integer());
            EXPECT_EQ(arr->get_value_at(5)->to_integer()->get_integer(), 1);
            ASSERT_TRUE(arr->get_value_at(6));
            ASSERT_TRUE(arr->get_value_at(6)->to_float());
            EXPECT_EQ(arr->get_value_at(6)->to_float()->get_float(), 2.3);
            ASSERT_TRUE(arr->get_value_at(7));
            ASSERT_TRUE(arr->get_value_at(7)->to_none());
        });
}

//------------------------------------------------------------------------------
// DOCUMENTS WITH COMMENTS AND WHITESPACE
//------------------------------------------------------------------------------

TEST(ParseCompleteDocuments, DocumentsWithComments) {
    test_parse_both(
        "// This is a configuration file\n"
        "{\n"
        "  // Database configuration\n"
        "  \"database\": {\n"
        "    \"host\": \"localhost\", // Default host\n"
        "    \"port\": 5432 // PostgreSQL default port\n"
        "  },\n"
        "  // Application settings\n"
        "  \"app\": {\n"
        "    \"debug\": true, // Enable debug mode\n"
        "    \"version\": \"1.0.0\" // Current version\n"
        "  }\n"
        "  // End of configuration\n"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);
            EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

            auto obj = result->to_object();
            ASSERT_TRUE(obj);
            EXPECT_EQ(obj->size(), 2);

            auto db = obj->get_value_for("database")->to_object();
            EXPECT_EQ(db->get_value_for("host")->to_string()->get_string(), "localhost");
            EXPECT_EQ(db->get_value_for("port")->to_integer()->get_integer(), 5432);

            auto app = obj->get_value_for("app")->to_object();
            EXPECT_TRUE(app->get_value_for("debug")->to_boolean()->get_boolean());
            EXPECT_EQ(app->get_value_for("version")->to_string()->get_string(), "1.0.0");
        });
}

TEST(ParseCompleteDocuments, DocumentsWithVariousWhitespace) {

    // Document with excessive whitespace
    test_parse_both(
        "\n\n\n   {\n\n\n"
        "   \"a\"   :   1   ,\n\n"
        "   \"b\"   :   [   2   ,   3   ]   ,\n\n"
        "   \"c\"   :   {   \"d\"   :   4   }\n\n"
        "}\n\n\n",
        [](auto, auto spaced) {
            ASSERT_TRUE(spaced);
            EXPECT_EQ(spaced->get_type(), jayson::jayson_types::OBJECT);

            // Document with minimal whitespace
            test_parse_both("{\"a\":1,\"b\":[2,3],\"c\":{\"d\":4}}", [&](auto, auto compact) {
                ASSERT_TRUE(compact);
                EXPECT_EQ(compact->get_type(), jayson::jayson_types::OBJECT);

                // Both should parse to equivalent structures
                auto compact_obj = compact->to_object();
                auto spaced_obj  = spaced->to_object();
                EXPECT_EQ(compact_obj->size(), spaced_obj->size());
            });
        });
}

//------------------------------------------------------------------------------
// REALISTIC JSON DOCUMENTS
//------------------------------------------------------------------------------

TEST(ParseCompleteDocuments, APIResponseDocument) {
    test_parse_both(
        "{\n"
        "  \"status\": \"success\",\n"
        "  \"data\": {\n"
        "    \"users\": [\n"
        "      {\n"
        "        \"id\": 1,\n"
        "        \"username\": \"alice\",\n"
        "        \"email\": \"alice@example.com\",\n"
        "        \"active\": true,\n"
        "        \"roles\": [\"user\", \"admin\"],\n"
        "        \"created_at\": \"2023-01-15T10:30:00Z\",\n"
        "        \"profile\": {\n"
        "          \"first_name\": \"Alice\",\n"
        "          \"last_name\": \"Smith\",\n"
        "          \"avatar_url\": null\n"
        "        }\n"
        "      },\n"
        "      {\n"
        "        \"id\": 2,\n"
        "        \"username\": \"bob\",\n"
        "        \"email\": \"bob@example.com\",\n"
        "        \"active\": false,\n"
        "        \"roles\": [\"user\"],\n"
        "        \"created_at\": \"2023-02-20T14:45:00Z\",\n"
        "        \"profile\": {\n"
        "          \"first_name\": \"Bob\",\n"
        "          \"last_name\": \"Johnson\",\n"
        "          \"avatar_url\": \"https://example.com/avatars/bob.jpg\"\n"
        "        }\n"
        "      }\n"
        "    ],\n"
        "    \"pagination\": {\n"
        "      \"page\": 1,\n"
        "      \"per_page\": 10,\n"
        "      \"total\": 2,\n"
        "      \"has_more\": false\n"
        "    }\n"
        "  },\n"
        "  \"meta\": {\n"
        "    \"timestamp\": \"2023-03-15T12:00:00Z\",\n"
        "    \"version\": \"v1.2.3\"\n"
        "  }\n"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            auto root = result->to_object();
            EXPECT_EQ(root->get_value_for("status")->to_string()->get_string(), "success");

            auto data  = root->get_value_for("data")->to_object();
            auto users = data->get_value_for("users")->to_array();
            EXPECT_EQ(users->size(), 2);

            // Check first user
            auto user1 = users->get_value_at(0)->to_object();
            EXPECT_EQ(user1->get_value_for("id")->to_integer()->get_integer(), 1);
            EXPECT_EQ(user1->get_value_for("username")->to_string()->get_string(), "alice");
            EXPECT_TRUE(user1->get_value_for("active")->to_boolean()->get_boolean());

            auto user1_roles = user1->get_value_for("roles")->to_array();
            EXPECT_EQ(user1_roles->size(), 2);
            EXPECT_EQ(user1_roles->get_value_at(0)->to_string()->get_string(), "user");
            EXPECT_EQ(user1_roles->get_value_at(1)->to_string()->get_string(), "admin");

            // Check pagination
            auto pagination = data->get_value_for("pagination")->to_object();
            EXPECT_EQ(pagination->get_value_for("page")->to_integer()->get_integer(), 1);
            EXPECT_FALSE(pagination->get_value_for("has_more")->to_boolean()->get_boolean());
        });
}

TEST(ParseCompleteDocuments, ConfigurationDocument) {
    test_parse_both(
        "{\n"
        "  // Application configuration\n"
        "  \"app_name\": \"MyApp\",\n"
        "  \"version\": \"2.1.0\",\n"
        "  \"environment\": \"production\",\n"
        "  \n"
        "  // Database settings\n"
        "  \"database\": {\n"
        "    \"primary\": {\n"
        "      \"host\": \"db-primary.example.com\",\n"
        "      \"port\": 5432,\n"
        "      \"name\": \"myapp_prod\",\n"
        "      \"ssl\": true,\n"
        "      \"pool_size\": 20\n"
        "    },\n"
        "    \"replica\": {\n"
        "      \"host\": \"db-replica.example.com\",\n"
        "      \"port\": 5432,\n"
        "      \"name\": \"myapp_prod\",\n"
        "      \"ssl\": true,\n"
        "      \"pool_size\": 10\n"
        "    }\n"
        "  },\n"
        "  \n"
        "  // Cache configuration\n"
        "  \"cache\": {\n"
        "    \"redis\": {\n"
        "      \"nodes\": [\n"
        "        {\"host\": \"redis-1.example.com\", \"port\": 6379},\n"
        "        {\"host\": \"redis-2.example.com\", \"port\": 6379},\n"
        "        {\"host\": \"redis-3.example.com\", \"port\": 6379}\n"
        "      ],\n"
        "      \"ttl\": 3600,\n"
        "      \"key_prefix\": \"myapp:\"\n"
        "    }\n"
        "  },\n"
        "  \n"
        "  // Feature flags\n"
        "  \"features\": {\n"
        "    \"new_ui\": true,\n"
        "    \"beta_features\": false,\n"
        "    \"maintenance_mode\": false,\n"
        "    \"rate_limiting\": {\n"
        "      \"enabled\": true,\n"
        "      \"requests_per_minute\": 100\n"
        "    }\n"
        "  }\n"
        "}",
        [](auto, auto result) {
            ASSERT_TRUE(result);

            auto config = result->to_object();
            EXPECT_EQ(config->get_value_for("app_name")->to_string()->get_string(), "MyApp");
            EXPECT_EQ(
                config->get_value_for("environment")->to_string()->get_string(), "production");

            // Check database configuration
            auto db      = config->get_value_for("database")->to_object();
            auto primary = db->get_value_for("primary")->to_object();
            EXPECT_EQ(primary->get_value_for("host")->to_string()->get_string(),
                "db-primary.example.com");
            EXPECT_TRUE(primary->get_value_for("ssl")->to_boolean()->get_boolean());

            // Check cache configuration
            auto cache = config->get_value_for("cache")->to_object();
            auto redis = cache->get_value_for("redis")->to_object();
            auto nodes = redis->get_value_for("nodes")->to_array();
            EXPECT_EQ(nodes->size(), 3);

            auto node1 = nodes->get_value_at(0)->to_object();
            EXPECT_EQ(
                node1->get_value_for("host")->to_string()->get_string(), "redis-1.example.com");
            EXPECT_EQ(node1->get_value_for("port")->to_integer()->get_integer(), 6379);
        });
}

//------------------------------------------------------------------------------
// REAL-WORLD DATASET
//------------------------------------------------------------------------------
TEST(ParseCompleteDocuments, RealWorldDatasetExcerpt) {
    std::ifstream t("tests/red-dress.jayson");
    std::stringstream buffer;
    buffer << t.rdbuf();

    auto input = buffer.str();
    auto tokens = tokenize_all(input);
    ASSERT_EQ(tokens.size(), 302739);
    test_parse_both(input, [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

        // Navigate the complex structure to verify correct parsing
        auto root = result->to_object();
        ASSERT_TRUE(root);

        auto network_interface = root->get_value_for("network_interface");
        ASSERT_TRUE(network_interface);
        EXPECT_EQ(network_interface->get_type(), jayson::jayson_types::OBJECT);

        auto network = network_interface->to_object()->get_value_for("network");
        ASSERT_TRUE(network);

        // Check exports array
        auto exports = network->to_object()->get_value_for("exports");
        ASSERT_TRUE(exports);
        EXPECT_EQ(exports->get_type(), jayson::jayson_types::ARRAY);

        auto exports_arr = exports->to_array();
        EXPECT_EQ(exports_arr->size(), 1);

        auto export_node = exports_arr->get_value_at(0)->to_object()->get_value_for("Node")->to_object();
        EXPECT_EQ(export_node->get_value_for("node_id")->to_integer()->get_integer(), 239476273194337494LL);
        EXPECT_EQ(export_node->get_value_for("output_index")->to_integer()->get_integer(), 0);
        EXPECT_FALSE(export_node->get_value_for("lambda")->to_boolean()->get_boolean());

        // Check nodes array structure
        auto nodes = network->to_object()->get_value_for("nodes");
        ASSERT_TRUE(nodes);
        EXPECT_EQ(nodes->get_type(), jayson::jayson_types::ARRAY);

        auto nodes_arr = nodes->to_array();
        EXPECT_EQ(nodes_arr->size(), 359);

        // Verify deep nested structure parses correctly
        auto node_entry = nodes_arr->get_value_at(0)->to_array();
        EXPECT_EQ(node_entry->size(), 2);

        // Navigate deep into the VectorData structure
        auto node_data = node_entry->get_value_at(1)->to_object();
        auto inputs    = node_data->get_value_for("inputs")->to_array();
        auto value     = inputs->get_value_at(0)->to_object()->get_value_for("Value")->to_object();
        auto tagged_value = value->get_value_for("tagged_value")->to_object();
        auto vector_data  = tagged_value->get_value_for("VectorData")->to_object();

        // Check color values
        auto instance = vector_data->get_value_for("instance")->to_array();
        auto style    = instance->get_value_at(0)->to_object()->get_value_for("style")->to_object();
        auto stroke   = style->get_value_for("stroke")->to_object();
        auto color    = stroke->get_value_for("color")->to_object();

        EXPECT_DOUBLE_EQ(color->get_value_for("red")->to_float()->get_float(), 0.0);
        EXPECT_DOUBLE_EQ(color->get_value_for("green")->to_float()->get_float(), 0.0);
        EXPECT_DOUBLE_EQ(color->get_value_for("blue")->to_float()->get_float(), 0.0);
        EXPECT_DOUBLE_EQ(color->get_value_for("alpha")->to_float()->get_float(), 1.0);

        // Check transform array
        auto transform = stroke->get_value_for("transform")->to_array();
        EXPECT_EQ(transform->size(), 6);
        EXPECT_DOUBLE_EQ(transform->get_value_at(0)->to_float()->get_float(), 1.0);
    });
}

//------------------------------------------------------------------------------
// INVALID COMPLETE DOCUMENTS
//------------------------------------------------------------------------------

TEST(ParseCompleteDocuments, EmptyDocument) {
    // Completely empty should fail
    check_parse_fails("");
    check_parse_fails("   ");
    check_parse_fails("\t\n\r");

    // Only comments should fail
    check_parse_fails("// just a comment");
    check_parse_fails("// comment 1\n// comment 2");
}

TEST(ParseCompleteDocuments, IncompleteDocuments) {
    // Truncated documents
    check_parse_fails("{\"key\": ");
    check_parse_fails("[1, 2, ");
    check_parse_fails("{\"incomplete\"");
    check_parse_fails("\"unterminated string");

    // Missing closing delimiters
    check_parse_fails("{\"key\": \"value\"");
    check_parse_fails("[1, 2, 3");
    check_parse_fails("{\"nested\": {\"inner\": \"value\"}");
}

//------------------------------------------------------------------------------
// LARGE DOCUMENTS
//------------------------------------------------------------------------------


TEST(ParseCompleteDocuments, LargeDocument) {
    // Generate a reasonably large JSON document
    std::string large_json = "{\n";
    large_json += "  \"metadata\": {\n";
    large_json += "    \"generated\": true,\n";
    large_json += "    \"size\": \"large\"\n";
    large_json += "  },\n";
    large_json += "  \"data\": [\n";

    for (int i = 0; i < 100; ++i) {
        if (i > 0) large_json += ",\n";
        large_json += "    {\n";
        large_json += "      \"id\": " + std::to_string(i) + ",\n";
        large_json += "      \"name\": \"item_" + std::to_string(i) + "\",\n";
        large_json += "      \"active\": ";
        large_json += (i % 2 == 0 ? "true" : "false");
        large_json += ",\n";
        large_json += "      \"tags\": [";
        for (int j = 0; j < 3; ++j) {
            if (j > 0) large_json += ", ";
            large_json += "\"tag_" + std::to_string(j) + "\"";
        }
        large_json += "]\n";
        large_json += "    }";
    }

    large_json += "\n  ]\n}";

    test_parse_both(large_json, [](auto, auto result) {
        ASSERT_TRUE(result);
        EXPECT_EQ(result->get_type(), jayson::jayson_types::OBJECT);

        auto root = result->to_object();
        auto data = root->get_value_for("data")->to_array();
        EXPECT_EQ(data->size(), 100);

        // Spot check a few items
        auto item_0 = data->get_value_at(0)->to_object();
        EXPECT_EQ(item_0->get_value_for("id")->to_integer()->get_integer(), 0);
        EXPECT_EQ(item_0->get_value_for("name")->to_string()->get_string(), "item_0");
        EXPECT_TRUE(item_0->get_value_for("active")->to_boolean()->get_boolean());

        auto item_99 = data->get_value_at(99)->to_object();
        EXPECT_EQ(item_99->get_value_for("id")->to_integer()->get_integer(), 99);
        EXPECT_EQ(item_99->get_value_for("name")->to_string()->get_string(), "item_99");
        EXPECT_FALSE(item_99->get_value_for("active")->to_boolean()->get_boolean());
    });
}
