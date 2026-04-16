#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

import collab.core;

using namespace collab::model;

// ═════════════════════════════════════════════════════════════════════════
// Test structs
// ═════════════════════════════════════════════════════════════════════════

struct SchemaTestDog {
    field<std::string> name;
    field<int>         age;
};

struct PlainCat {
    std::string name;
    int         age = 0;
};

#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::model::reflect_on<SchemaTestDog>() {
    return collab::model::field_info<SchemaTestDog>("name", "age");
}

template <>
constexpr auto collab::model::reflect_on<PlainCat>() {
    return collab::model::field_info<PlainCat>("name", "age");
}
#endif

// ═════════════════════════════════════════════════════════════════════════
// Static assertions: all three modes satisfy type_schema
// ═════════════════════════════════════════════════════════════════════════

static_assert(type_schema<type_def<SchemaTestDog>>,
    "typed type_def<T> must satisfy type_schema");

static_assert(type_schema<type_def<dynamic_tag>>,
    "dynamic type_def must satisfy type_schema");

// ═════════════════════════════════════════════════════════════════════════
// Tests: generic function constrained by type_schema
// ═════════════════════════════════════════════════════════════════════════

std::string schema_summary(const type_schema auto& t) {
    std::string result = std::string(t.name()) + ": ";
    auto names = t.field_names();
    for (std::size_t i = 0; i < names.size(); ++i) {
        if (i > 0) result += ", ";
        result += names[i];
    }
    return result;
}

TEST_CASE("type_schema works with typed type_def", "[type_schema]") {
    type_def<SchemaTestDog> t;
    auto summary = schema_summary(t);
    REQUIRE(summary == "SchemaTestDog: name, age");
}

TEST_CASE("type_schema works with dynamic type_def", "[type_schema]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count");
    auto summary = schema_summary(t);
    REQUIRE(summary == "Event: title, count");
}

TEST_CASE("type_schema works with hybrid type_def", "[type_schema]") {
    auto t = type_def<PlainCat>()
        .field(&PlainCat::name, "name")
        .field(&PlainCat::age, "age");
    auto summary = schema_summary(t);
    REQUIRE(summary == "PlainCat: name, age");
}

TEST_CASE("type_schema has_field works generically", "[type_schema]") {
    auto check_has = [](const type_schema auto& t, std::string_view name) {
        return t.has_field(name);
    };

    type_def<SchemaTestDog> typed;
    REQUIRE(check_has(typed, "name"));
    REQUIRE(!check_has(typed, "nope"));

    auto dynamic = type_def("Thing")
        .field<int>("x");
    REQUIRE(check_has(dynamic, "x"));
    REQUIRE(!check_has(dynamic, "nope"));

    auto hybrid = type_def<PlainCat>()
        .field(&PlainCat::name, "name");
    REQUIRE(check_has(hybrid, "name"));
    REQUIRE(!check_has(hybrid, "nope"));
}
