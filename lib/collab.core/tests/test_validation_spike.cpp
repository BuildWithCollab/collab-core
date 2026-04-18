#include <catch2/catch_test_macros.hpp>

#include <nlohmann/json.hpp>

#include <string>

import collab.core;

using namespace collab::model;
using namespace collab::model::validations;

// ═════════════════════════════════════════════════════════════════════════
// Spike Test 1: single validator on dynamic field, valid() returns false
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("spike: single validator on dynamic field", "[validation][spike]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string(""),
            validators(not_empty{}));

    auto dog = dog_type.create();
    // name is "" — not_empty should fail

    REQUIRE(!dog.valid());
}

// ═════════════════════════════════════════════════════════════════════════
// Spike Test 2: validate() returns errors with details
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("spike: validate() returns errors with details", "[validation][spike]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string(""),
            validators(not_empty{}));

    auto dog = dog_type.create();
    auto result = dog.validate();

    REQUIRE(!result);
    REQUIRE(result.error_count() == 1);
    REQUIRE(result.errors()[0].path == "name");
    REQUIRE(result.errors()[0].constraint == "not_empty");
    REQUIRE(!result.errors()[0].message.empty());
}

// ═════════════════════════════════════════════════════════════════════════
// Spike Test 3: multiple validators on one field
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("spike: multiple validators on one field", "[validation][spike]") {
    auto config_type = type_def("Config")
        .field<std::string>("token", std::string(""),
            validators(not_empty{}, max_length{10}));

    auto config = config_type.create();
    config.set("token", std::string("this-is-way-too-long-for-the-limit"));

    auto result = config.validate();
    // not_empty passes (it's not empty), but max_length fails
    REQUIRE(result.error_count() == 1);

    // Now set it empty — not_empty fails, max_length still passes (0 <= 10)
    config.set("token", std::string(""));
    auto result2 = config.validate();
    REQUIRE(result2.error_count() == 1);

    // Set to something long AND valid for not_empty — only max_length fails
    config.set("token", std::string("abcdefghijk"));  // 11 chars > 10
    auto result3 = config.validate();
    REQUIRE(result3.error_count() == 1);

    // Set to valid for both — no errors
    config.set("token", std::string("ok"));
    auto result4 = config.validate();
    REQUIRE(result4.ok());
}

// ═════════════════════════════════════════════════════════════════════════
// Spike Test 4: fields without validators always pass
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("spike: fields without validators always pass", "[validation][spike]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string(""),
            validators(not_empty{}))
        .field<int>("age")            // no validators
        .field<std::string>("breed"); // no validators

    auto dog = dog_type.create();
    dog.set("age", 3);
    dog.set("breed", std::string("Husky"));
    // name is still "" — only that one should fail

    auto result = dog.validate();
    REQUIRE(result.error_count() == 1);
    REQUIRE(result.errors()[0].path == "name");
}

// ═════════════════════════════════════════════════════════════════════════
// Spike Test 5: fixing values makes validation pass
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("spike: valid() becomes true after fixing values", "[validation][spike]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string(""),
            validators(not_empty{}))
        .field<int>("age", 0,
            validators(positive{}));

    auto dog = dog_type.create();
    REQUIRE(!dog.valid());

    dog.set("name", std::string("Rex"));
    REQUIRE(!dog.valid()); // age still 0

    dog.set("age", 3);
    REQUIRE(dog.valid()); // both pass now

    auto result = dog.validate();
    REQUIRE(result.ok());
    REQUIRE(result.error_count() == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Spike Test 6: validators don't interfere with normal field operations
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("spike: validators don't affect set/get/to_json", "[validation][spike]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string(""),
            validators(not_empty{}))
        .field<int>("age", 0,
            validators(positive{}));

    auto dog = dog_type.create();

    // Invalid values can still be set and retrieved
    dog.set("name", std::string(""));
    REQUIRE(dog.get<std::string>("name") == "");

    // to_json still works even when invalid
    auto j = dog.to_json();
    REQUIRE(j["name"] == "");
    REQUIRE(j["age"] == 0);

    // Schema queries unaffected
    REQUIRE(dog.type().field_count() == 2);
    REQUIRE(dog.type().field_names()[0] == "name");
}

// ═════════════════════════════════════════════════════════════════════════
// Validators mixed with metas in the same .field() call
// ═════════════════════════════════════════════════════════════════════════

struct spike_help_info {
    const char* summary = "";
};

struct spike_cli_meta {
    struct { char short_flag = '\0'; } cli;
};

TEST_CASE("spike: validators and with<> metas in same field call", "[validation][dynamic]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string(""),
            validators(not_empty{}),
            with<spike_help_info>({.summary = "Dog's name"}))
        .field<int>("age", 0,
            validators(positive{}),
            with<spike_cli_meta>({.cli = {.short_flag = 'a'}}),
            with<spike_help_info>({.summary = "Age in years"}));

    // Validators work
    auto dog = dog_type.create();
    REQUIRE(!dog.valid());

    dog.set("name", std::string("Rex"));
    dog.set("age", 3);
    REQUIRE(dog.valid());

    // Metas are accessible
    REQUIRE(dog_type.field("name").has_meta<spike_help_info>());
    REQUIRE(std::string_view{dog_type.field("name").meta<spike_help_info>().summary} == "Dog's name");
    REQUIRE(dog_type.field("age").has_meta<spike_cli_meta>());
    REQUIRE(dog_type.field("age").meta<spike_cli_meta>().cli.short_flag == 'a');
    REQUIRE(dog_type.field("age").has_meta<spike_help_info>());
}

TEST_CASE("spike: metas before validators in same field call", "[validation][dynamic]") {
    // Order shouldn't matter — metas first, then validators
    auto config_type = type_def("Config")
        .field<std::string>("token", std::string("secret"),
            with<spike_help_info>({.summary = "API token"}),
            validators(not_empty{}, max_length{50}));

    auto config = config_type.create();
    REQUIRE(config.valid());

    // Meta still accessible
    REQUIRE(config.type().field("token").has_meta<spike_help_info>());
    REQUIRE(std::string_view{config.type().field("token").meta<spike_help_info>().summary} == "API token");

    // Validator still works
    config.set("token", std::string(""));
    REQUIRE(!config.valid());
}

// ═════════════════════════════════════════════════════════════════════════
// Custom user-defined validators
// ═════════════════════════════════════════════════════════════════════════

struct starts_with_uppercase {
    std::optional<std::string> operator()(const std::string& value) const {
        if (value.empty() || std::isupper(static_cast<unsigned char>(value[0])))
            return std::nullopt;
        return std::format("'{}' must start with an uppercase letter", value);
    }
};

struct in_range {
    int minimum;
    int maximum;
    std::optional<std::string> operator()(int value) const {
        if (value >= minimum && value <= maximum)
            return std::nullopt;
        return std::format("{} must be between {} and {}", value, minimum, maximum);
    }
};

TEST_CASE("spike: custom validator — basic contract", "[validation][dynamic][custom]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string("rex"),
            validators(starts_with_uppercase{}));

    auto dog = dog_type.create();
    REQUIRE(!dog.valid());

    auto result = dog.validate();
    REQUIRE(result.error_count() == 1);
    REQUIRE(result.errors()[0].path == "name");
    REQUIRE(result.errors()[0].constraint == "starts_with_uppercase");
    REQUIRE(result.errors()[0].message.find("uppercase") != std::string::npos);

    dog.set("name", std::string("Rex"));
    REQUIRE(dog.valid());
}

TEST_CASE("spike: custom validator with parameters", "[validation][dynamic][custom]") {
    auto dog_type = type_def("Dog")
        .field<int>("age", 0,
            validators(in_range{.minimum = 0, .maximum = 30}));

    auto dog = dog_type.create();
    REQUIRE(dog.valid());  // 0 is in range [0, 30]

    dog.set("age", 31);
    REQUIRE(!dog.valid());

    auto result = dog.validate();
    REQUIRE(result.errors()[0].constraint == "in_range");
    REQUIRE(result.errors()[0].message.find("30") != std::string::npos);

    dog.set("age", 15);
    REQUIRE(dog.valid());
}

TEST_CASE("spike: custom validators mixed with built-ins", "[validation][dynamic][custom]") {
    auto dog_type = type_def("Dog")
        .field<std::string>("name", std::string(""),
            validators(not_empty{}, starts_with_uppercase{}))
        .field<int>("age", 0,
            validators(positive{}, in_range{.minimum = 1, .maximum = 30}));

    auto dog = dog_type.create();

    // Empty name fails not_empty (starts_with_uppercase passes on empty by design)
    auto result = dog.validate();
    REQUIRE(result.error_count() >= 2);  // name: not_empty, age: positive + in_range overlap

    dog.set("name", std::string("Rex"));
    dog.set("age", 5);
    REQUIRE(dog.valid());
}
