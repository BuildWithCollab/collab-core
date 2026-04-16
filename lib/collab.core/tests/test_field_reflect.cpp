#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>
#include <vector>

import collab.core;

using namespace collab::field;

// ── Forward declarations for reflect_on fallbacks ──────────────────────

struct SimpleArgs;
struct WeatherArgs;
struct MixedStruct;

#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::field::reflect_on<SimpleArgs>() {
    return collab::field::field_info<SimpleArgs>("name", "age", "active");
}

template <>
constexpr auto collab::field::reflect_on<WeatherArgs>() {
    return collab::field::field_info<WeatherArgs>("city", "days", "verbose", "tags");
}

template <>
constexpr auto collab::field::reflect_on<MixedStruct>() {
    return collab::field::field_info<MixedStruct>("visible", "counter", "score");
}
#endif

// ── Test structs ────────────────────────────────────────────────────────

struct posix_options {
    char short_flag = '\0';
    bool from_stdin = false;
};

struct render_options {
    const char* style = "";
    int         width = 0;
};

struct posix_meta  { posix_options posix{}; };
struct render_meta { render_options render{}; };

struct SimpleArgs {
    field<std::string> name;
    field<int>         age;
    field<bool>        active;
};

struct WeatherArgs {
    field<std::string>                       city;
    field<int>                               days    {.value = 7};
    field<bool, with<posix_meta>>            verbose {.with = {{.posix = {.short_flag = 'v'}}}};
    field<std::vector<std::string>>          tags;
};

struct MixedStruct {
    field<std::string> visible;
    int                counter = 0;
    field<int>         score;
};

// ── type_name ───────────────────────────────────────────────────────────

TEST_CASE("type_name extracts struct name", "[reflect][type_name]") {
    constexpr auto name = type_name<SimpleArgs>();
    REQUIRE(name == "SimpleArgs");

    constexpr auto wname = type_name<WeatherArgs>();
    REQUIRE(wname == "WeatherArgs");
}

TEST_CASE("type_name works for primitives", "[reflect][type_name]") {
    constexpr auto iname = type_name<int>();
    REQUIRE(iname == "int");
}

// ── reflect<T>().name() ─────────────────────────────────────────────────

TEST_CASE("reflection name() returns type name", "[reflect]") {
    constexpr auto schema = reflect<SimpleArgs>();
    REQUIRE(schema.name() == "SimpleArgs");
}

// ── reflect<T>().count() ────────────────────────────────────────────────

TEST_CASE("reflection count() returns Field member count", "[reflect]") {
    constexpr auto schema = reflect<SimpleArgs>();
    REQUIRE(schema.count() == 3);

    constexpr auto wschema = reflect<WeatherArgs>();
    REQUIRE(wschema.count() == 4);

    // MixedStruct has 3 total members but only 2 are field<>
    constexpr auto mschema = reflect<MixedStruct>();
    REQUIRE(mschema.count() == 2);
}

// ── reflect<T>().names() ────────────────────────────────────────────────

TEST_CASE("reflection names() returns Field member names", "[reflect]") {
    auto names = reflect<SimpleArgs>().names();
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "active");
}

TEST_CASE("reflection names() for WeatherArgs", "[reflect]") {
    auto names = reflect<WeatherArgs>().names();
    REQUIRE(names[0] == "city");
    REQUIRE(names[1] == "days");
    REQUIRE(names[2] == "verbose");
    REQUIRE(names[3] == "tags");
}

TEST_CASE("reflection names() filters to Field members only", "[reflect]") {
    auto names = reflect<MixedStruct>().names();
    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "visible");
    REQUIRE(names[1] == "score");
    // "counter" (plain int) should NOT appear
}

// ── reflect<T>().at<I>() — field descriptor ─────────────────────────────

TEST_CASE("field_descriptor name and index", "[reflect][at]") {
    constexpr auto f0 = reflect<WeatherArgs>().at<0>();
    REQUIRE(f0.name() == "city");
    REQUIRE(f0.index() == 0);

    constexpr auto f2 = reflect<WeatherArgs>().at<2>();
    REQUIRE(f2.name() == "verbose");
    REQUIRE(f2.index() == 2);
}

TEST_CASE("field_descriptor has_extension", "[reflect][at][extension]") {
    constexpr auto schema = reflect<WeatherArgs>();

    // city — no extensions
    REQUIRE(!schema.at<0>().has_extension<posix_meta>());

    // verbose — has posix_meta
    REQUIRE(schema.at<2>().has_extension<posix_meta>());
    REQUIRE(!schema.at<2>().has_extension<render_meta>());
}

TEST_CASE("field_descriptor default metadata via with()", "[reflect][at][with]") {
    constexpr auto schema = reflect<WeatherArgs>();

    // verbose's default short_flag is 'v'
    auto w = schema.at<2>().with();
    REQUIRE(w.posix.short_flag == 'v');
}

TEST_CASE("field_descriptor default value via value()", "[reflect][at][value]") {
    constexpr auto schema = reflect<WeatherArgs>();

    // days defaults to 7
    auto v = schema.at<1>().value();
    REQUIRE(v == 7);
}

// ── Free functions ──────────────────────────────────────────────────────

TEST_CASE("field_count free function", "[reflect][free]") {
    REQUIRE(field_count<SimpleArgs>() == 3);
    REQUIRE(field_count<WeatherArgs>() == 4);
}

TEST_CASE("field_name free function", "[reflect][free]") {
    REQUIRE(field_name<0, SimpleArgs>() == "name");
    REQUIRE(field_name<1, SimpleArgs>() == "age");
    REQUIRE(field_name<2, SimpleArgs>() == "active");
}

TEST_CASE("field_names free function", "[reflect][free]") {
    auto names = field_names<SimpleArgs>();
    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "name");
}

// ── for_each with instance ──────────────────────────────────────────────

TEST_CASE("for_each iterates Field members with values", "[reflect][for_each]") {
    SimpleArgs args;
    args.name = "Alice";
    args.age = 30;
    args.active = true;

    std::vector<std::string> collected_names;
    reflect<SimpleArgs>().for_each(args, [&](auto field) {
        collected_names.emplace_back(field.name());
    });

    REQUIRE(collected_names.size() == 3);
    REQUIRE(collected_names[0] == "name");
    REQUIRE(collected_names[1] == "age");
    REQUIRE(collected_names[2] == "active");
}

TEST_CASE("for_each provides mutable value access", "[reflect][for_each]") {
    SimpleArgs args;
    args.name = "Bob";

    reflect<SimpleArgs>().for_each(args, [](auto field) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field.value())>, std::string>) {
            field.value() = "Changed";
        }
    });

    REQUIRE(args.name.value == "Changed");
}

TEST_CASE("for_each skips non-Field members", "[reflect][for_each]") {
    MixedStruct ms;
    ms.visible = "hello";
    ms.counter = 999;
    ms.score = 42;

    std::vector<std::string> names;
    reflect<MixedStruct>().for_each(ms, [&](auto field) {
        names.emplace_back(field.name());
    });

    // Should only see "visible" and "score", not "counter"
    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "visible");
    REQUIRE(names[1] == "score");
}

TEST_CASE("for_each with extension access", "[reflect][for_each][extension]") {
    WeatherArgs args;

    char found_flag = '\0';
    reflect<WeatherArgs>().for_each(args, [&](auto field) {
        if constexpr (field.template has_extension<posix_meta>()) {
            found_flag = field.with().posix.short_flag;
        }
    });

    REQUIRE(found_flag == 'v');
}

// ── get field by runtime name ───────────────────────────────────────────────

TEST_CASE("get finds field by name", "[reflect][get]") {
    SimpleArgs args;
    args.age = 25;

    int found_value = 0;
    bool found = reflect<SimpleArgs>().get(args, "age", [&](auto field) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field.value())>, int>) {
            found_value = field.value();
        }
    });

    REQUIRE(found);
    REQUIRE(found_value == 25);
}

TEST_CASE("get returns false for unknown name", "[reflect][get]") {
    SimpleArgs args;

    bool found = reflect<SimpleArgs>().get(args, "nonexistent", [](auto) {});

    REQUIRE(!found);
}

// ── reflect(instance) deduces T ─────────────────────────────────────────

TEST_CASE("reflect(instance) deduces type", "[reflect]") {
    const SimpleArgs args{};

    auto schema = reflect(args);
    REQUIRE(schema.name() == "SimpleArgs");
    REQUIRE(schema.count() == 3);
}

// JSON round-trip is tested in test_Field_json.cpp — no need to duplicate here.
