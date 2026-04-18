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
