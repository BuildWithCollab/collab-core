#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

import collab.core;

using namespace collab::model;

// ═════════════════════════════════════════════════════════════════════════
// Test metadata types
// ═════════════════════════════════════════════════════════════════════════

struct help_info_h {
    const char* summary = "";
};

struct render_meta_h {
    struct { const char* style = ""; int width = 0; } render;
};

// ═════════════════════════════════════════════════════════════════════════
// Test structs — plain C++ structs, no field<> or meta<>
// ═════════════════════════════════════════════════════════════════════════

struct PlainDog {
    std::string name;
    int         age    = 0;
    std::string breed;
};

struct PlainPoint {
    double x = 0.0;
    double y = 0.0;
};

// ═════════════════════════════════════════════════════════════════════════
// reflect_on fallbacks (non-PFR builds)
// ═════════════════════════════════════════════════════════════════════════

#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::model::reflect_on<PlainDog>() {
    return collab::model::field_info<PlainDog>("name", "age", "breed");
}

template <>
constexpr auto collab::model::reflect_on<PlainPoint>() {
    return collab::model::field_info<PlainPoint>("x", "y");
}
#endif

// ═════════════════════════════════════════════════════════════════════════
// Tests: Hybrid construction and schema queries
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def name()", "[type_def][hybrid]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age");
    REQUIRE(t.name() == "PlainDog");
}

TEST_CASE("hybrid type_def field_count() includes registered fields", "[type_def][hybrid][field_count]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age")
        .field(&PlainDog::breed, "breed");
    // PlainDog has no field<> members, so auto-discovered count is 0
    // 3 hybrid registered fields
    REQUIRE(t.field_count() == 3);
}

TEST_CASE("hybrid type_def field_names() includes registered fields", "[type_def][hybrid][field_names]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age");
    auto names = t.field_names();
    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: has_field() and field() queries
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def has_field()", "[type_def][hybrid][has_field]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age");
    REQUIRE(t.has_field("name"));
    REQUIRE(t.has_field("age"));
    REQUIRE(!t.has_field("breed"));
    REQUIRE(!t.has_field("nope"));
}

TEST_CASE("hybrid type_def field().name()", "[type_def][hybrid][field_query]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    REQUIRE(t.field("name").name() == "name");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Field-level metas via with<>
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def field with meta", "[type_def][hybrid][field_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name",
            with<help_info_h>({.summary = "Dog's name"}));
    REQUIRE(t.field("name").has_meta<help_info_h>());
    REQUIRE(std::string_view{t.field("name").meta<help_info_h>().summary} == "Dog's name");
}

TEST_CASE("hybrid type_def field with multiple metas", "[type_def][hybrid][field_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::age, "age",
            with<help_info_h>({.summary = "Age in years"}),
            with<render_meta_h>({.render = {.style = "bold", .width = 5}}));
    REQUIRE(t.field("age").has_meta<help_info_h>());
    REQUIRE(t.field("age").has_meta<render_meta_h>());
    REQUIRE(std::string_view{t.field("age").meta<help_info_h>().summary} == "Age in years");
    REQUIRE(t.field("age").meta<render_meta_h>().render.width == 5);
}

TEST_CASE("hybrid type_def field without meta", "[type_def][hybrid][field_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    REQUIRE(!t.field("name").has_meta<help_info_h>());
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: set() on hybrid registered fields
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def set() with matching type", "[type_def][hybrid][set]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age");
    PlainDog rex;

    REQUIRE(t.set(rex, "name", std::string("Rex")));
    REQUIRE(rex.name == "Rex");

    REQUIRE(t.set(rex, "age", 3));
    REQUIRE(rex.age == 3);
}

TEST_CASE("hybrid type_def set() returns false for unknown field", "[type_def][hybrid][set]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;

    REQUIRE(!t.set(rex, "nope", 42));
}

TEST_CASE("hybrid type_def set() returns false for type mismatch", "[type_def][hybrid][set]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;
    rex.name = "Original";

    REQUIRE(!t.set(rex, "name", 42));
    REQUIRE(rex.name == "Original");
}

TEST_CASE("hybrid type_def set() with const char* to string field", "[type_def][hybrid][set]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;

    REQUIRE(t.set(rex, "name", "Rex"));
    REQUIRE(rex.name == "Rex");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: get<T>() on hybrid registered fields
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def get<T>() returns value", "[type_def][hybrid][get]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age");
    PlainDog rex;
    rex.name = "Rex";
    rex.age = 3;

    REQUIRE(t.get<std::string>(rex, "name") == "Rex");
    REQUIRE(t.get<int>(rex, "age") == 3);
}

TEST_CASE("hybrid type_def get<T>() returns nullopt for unknown field", "[type_def][hybrid][get]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;

    REQUIRE(!t.get<std::string>(rex, "nope").has_value());
}

TEST_CASE("hybrid type_def get<T>() returns nullopt for type mismatch", "[type_def][hybrid][get]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::age, "age");
    PlainDog rex;
    rex.age = 5;

    REQUIRE(!t.get<std::string>(rex, "age").has_value());
}

TEST_CASE("hybrid type_def get<T>() round-trips with set()", "[type_def][hybrid][get][set]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age")
        .field(&PlainDog::breed, "breed");
    PlainDog rex;

    t.set(rex, "name", std::string("Rex"));
    t.set(rex, "age", 3);
    t.set(rex, "breed", std::string("Husky"));

    REQUIRE(t.get<std::string>(rex, "name") == "Rex");
    REQUIRE(t.get<int>(rex, "age") == 3);
    REQUIRE(t.get<std::string>(rex, "breed") == "Husky");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: create()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def create()", "[type_def][hybrid][create]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age");
    PlainDog dog = t.create();
    REQUIRE(dog.name.empty());
    REQUIRE(dog.age == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Pure auto-discovery still works (no hybrid fields)
// ═════════════════════════════════════════════════════════════════════════

struct FieldDog {
    field<std::string>  name;
    field<int>          age;
};

#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::model::reflect_on<FieldDog>() {
    return collab::model::field_info<FieldDog>("name", "age");
}
#endif

TEST_CASE("type_def<T> without hybrid still works", "[type_def][hybrid][compat]") {
    type_def<FieldDog> t;
    REQUIRE(t.field_count() == 2);
    REQUIRE(t.name() == "FieldDog");

    FieldDog rex;
    rex.name = "Rex";
    rex.age = 3;

    std::string found_name;
    t.for_each(rex, [&](std::string_view name, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>)
            if (name == "name") found_name = value;
    });
    REQUIRE(found_name == "Rex");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: for_each_field() on hybrid — iterates registered fields
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def for_each_field() iterates registered fields", "[type_def][hybrid][for_each_field]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age")
        .field(&PlainDog::breed, "breed");

    std::vector<std::string> names;
    t.for_each_field([&](auto descriptor) {
        names.emplace_back(descriptor.name());
    });

    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "breed");
}

TEST_CASE("hybrid type_def for_each_field() count matches field_count()", "[type_def][hybrid][for_each_field]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age");

    int visited = 0;
    t.for_each_field([&](auto) { ++visited; });
    REQUIRE(visited == static_cast<int>(t.field_count()));
}

TEST_CASE("hybrid type_def for_each_field() can query field metas", "[type_def][hybrid][for_each_field]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name",
            with<help_info_h>({.summary = "Dog's name"}))
        .field(&PlainDog::age, "age");

    bool name_has_help = false;
    bool age_has_help = true;

    t.for_each_field([&](auto descriptor) {
        if (descriptor.name() == "name")
            name_has_help = descriptor.template has_meta<help_info_h>();
        if (descriptor.name() == "age")
            age_has_help = descriptor.template has_meta<help_info_h>();
    });

    REQUIRE(name_has_help);
    REQUIRE(!age_has_help);
}

TEST_CASE("hybrid type_def for_each_field() reads meta values", "[type_def][hybrid][for_each_field]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name",
            with<help_info_h>({.summary = "Dog's name"}));

    std::string summary;
    t.for_each_field([&](auto descriptor) {
        if (descriptor.name() == "name" && descriptor.template has_meta<help_info_h>())
            summary = descriptor.template meta<help_info_h>().summary;
    });

    REQUIRE(summary == "Dog's name");
}

TEST_CASE("hybrid type_def for_each_field() on empty hybrid yields nothing", "[type_def][hybrid][for_each_field]") {
    auto t = type_def<PlainDog>();  // no fields registered
    int count = 0;
    t.for_each_field([&](auto) { ++count; });
    REQUIRE(count == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: for_each_meta() on hybrid
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def for_each_meta() on plain struct yields nothing", "[type_def][hybrid][for_each_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    int count = 0;
    t.for_each_meta([&](auto&) { ++count; });
    REQUIRE(count == 0);
}

TEST_CASE("hybrid type_def for_each_meta() on struct with metas", "[type_def][hybrid][for_each_meta]") {
    // FieldDog has field<> members but no meta<> members, so count should be 0
    type_def<FieldDog> t;
    int count = 0;
    t.for_each_meta([&](auto&) { ++count; });
    REQUIRE(count == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Full integration
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid type_def full integration", "[type_def][hybrid][integration]") {
    auto dog_t = type_def<PlainDog>()
        .field(&PlainDog::name, "name",
            with<help_info_h>({.summary = "Dog's name"}))
        .field(&PlainDog::age, "age",
            with<help_info_h>({.summary = "Age in years"}),
            with<render_meta_h>({.render = {.style = "bold"}}))
        .field(&PlainDog::breed, "breed");

    // Schema
    REQUIRE(dog_t.name() == "PlainDog");
    REQUIRE(dog_t.field_count() == 3);
    auto names = dog_t.field_names();
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "breed");

    // Field queries
    REQUIRE(dog_t.has_field("name"));
    REQUIRE(dog_t.has_field("breed"));
    REQUIRE(!dog_t.has_field("nope"));

    // Field metas
    REQUIRE(dog_t.field("name").has_meta<help_info_h>());
    REQUIRE(std::string_view{dog_t.field("name").meta<help_info_h>().summary} == "Dog's name");
    REQUIRE(dog_t.field("age").has_meta<help_info_h>());
    REQUIRE(dog_t.field("age").has_meta<render_meta_h>());
    REQUIRE(!dog_t.field("breed").has_meta<help_info_h>());

    // Create + set + get
    PlainDog rex = dog_t.create();
    REQUIRE(dog_t.set(rex, "name", std::string("Rex")));
    REQUIRE(dog_t.set(rex, "age", 3));
    REQUIRE(dog_t.set(rex, "breed", std::string("Husky")));

    REQUIRE(dog_t.get<std::string>(rex, "name") == "Rex");
    REQUIRE(dog_t.get<int>(rex, "age") == 3);
    REQUIRE(dog_t.get<std::string>(rex, "breed") == "Husky");

    // Type mismatch
    REQUIRE(!dog_t.set(rex, "name", 42));
    REQUIRE(!dog_t.get<int>(rex, "name").has_value());
}
