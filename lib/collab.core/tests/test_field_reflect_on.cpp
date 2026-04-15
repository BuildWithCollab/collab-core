#include <catch2/catch_test_macros.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <string_view>
#include <vector>

import collab.core;

using namespace collab::field;

// ── Test structs with reflect_on specializations ─────────────────────────
//
// These exercise the registration path (reflect_on → field_registry)
// even with PFR enabled, because has_reflect_on<T> is checked first.

struct RegSimple {
    field<std::string> name;
    field<int>         age;
    field<bool>        active;
};

template <>
constexpr auto collab::field::reflect_on<RegSimple>() {
    return collab::field::field_info<RegSimple>("name", "age", "active");
}

// ── Mixed: some field<> members, some plain ──────────────────────────────

struct RegMixed {
    field<std::string> label;
    int                counter = 0;
    field<int>         score;
};

template <>
constexpr auto collab::field::reflect_on<RegMixed>() {
    return collab::field::field_info<RegMixed>("label", "counter", "score");
}

// ── With extensions ──────────────────────────────────────────────────────

struct cli_meta {
    struct { char short_flag = '\0'; } cli;
};

struct RegWithExt {
    field<std::string>                   city;
    field<int>                           days    {.value = 5};
    field<bool, with<cli_meta>>          verbose {.with = {{.cli = {.short_flag = 'v'}}}};
    field<std::vector<std::string>>      tags;
};

template <>
constexpr auto collab::field::reflect_on<RegWithExt>() {
    return collab::field::field_info<RegWithExt>("city", "days", "verbose", "tags");
}

// ── Single field ─────────────────────────────────────────────────────────

struct RegSingle {
    field<int> value;
};

template <>
constexpr auto collab::field::reflect_on<RegSingle>() {
    return collab::field::field_info<RegSingle>("value");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests
// ═════════════════════════════════════════════════════════════════════════

// ── has_reflect_on detection ─────────────────────────────────────────────

TEST_CASE("has_reflect_on detects registered types", "[reflect_on]") {
    // These have specializations
    STATIC_REQUIRE(detail::has_reflect_on<RegSimple>);
    STATIC_REQUIRE(detail::has_reflect_on<RegMixed>);
    STATIC_REQUIRE(detail::has_reflect_on<RegWithExt>);
    STATIC_REQUIRE(detail::has_reflect_on<RegSingle>);
}

struct Unregistered {
    field<int> x;
};

TEST_CASE("has_reflect_on is false for unregistered types", "[reflect_on]") {
    STATIC_REQUIRE(!detail::has_reflect_on<Unregistered>);
}

// ── field_count via registration ─────────────────────────────────────────

TEST_CASE("field_count via reflect_on — all field<> members", "[reflect_on][count]") {
    REQUIRE(field_count<RegSimple>() == 3);
    REQUIRE(field_count<RegWithExt>() == 4);
    REQUIRE(field_count<RegSingle>() == 1);
}

TEST_CASE("field_count via reflect_on — mixed struct filters to field<> only", "[reflect_on][count]") {
    // RegMixed has 3 total members but only 2 are field<>
    REQUIRE(field_count<RegMixed>() == 2);
}

// ── field_name via registration ──────────────────────────────────────────

TEST_CASE("field_name via reflect_on", "[reflect_on][name]") {
    REQUIRE(field_name<0, RegSimple>() == "name");
    REQUIRE(field_name<1, RegSimple>() == "age");
    REQUIRE(field_name<2, RegSimple>() == "active");
}

TEST_CASE("field_name via reflect_on — with extensions", "[reflect_on][name]") {
    REQUIRE(field_name<0, RegWithExt>() == "city");
    REQUIRE(field_name<1, RegWithExt>() == "days");
    REQUIRE(field_name<2, RegWithExt>() == "verbose");
    REQUIRE(field_name<3, RegWithExt>() == "tags");
}

// ── field_names via registration ─────────────────────────────────────────

TEST_CASE("field_names via reflect_on returns field<> names only", "[reflect_on][names]") {
    constexpr auto names = field_names<RegSimple>();
    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "active");
}

TEST_CASE("field_names via reflect_on — mixed struct skips plain members", "[reflect_on][names]") {
    constexpr auto names = field_names<RegMixed>();
    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "label");
    REQUIRE(names[1] == "score");
    // "counter" (plain int) excluded
}

// ── reflection struct via registration ───────────────────────────────────

TEST_CASE("reflect<T>() via reflect_on — name and count", "[reflect_on][reflection]") {
    constexpr auto schema = reflect<RegSimple>();
    REQUIRE(schema.name() == "RegSimple");
    REQUIRE(schema.count() == 3);
}

TEST_CASE("reflect<T>().names() via reflect_on", "[reflect_on][reflection]") {
    constexpr auto names = reflect<RegSimple>().names();
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "active");
}

// ── field_descriptor via registration ────────────────────────────────────

TEST_CASE("field_descriptor via reflect_on — name and index", "[reflect_on][descriptor]") {
    constexpr auto f0 = reflect<RegWithExt>().at<0>();
    REQUIRE(f0.name() == "city");
    REQUIRE(f0.index() == 0);

    constexpr auto f2 = reflect<RegWithExt>().at<2>();
    REQUIRE(f2.name() == "verbose");
    REQUIRE(f2.index() == 2);
}

TEST_CASE("field_descriptor via reflect_on — has_extension", "[reflect_on][descriptor][extension]") {
    constexpr auto schema = reflect<RegWithExt>();

    // city — no extensions
    REQUIRE(!schema.at<0>().has_extension<cli_meta>());

    // verbose — has cli_meta
    REQUIRE(schema.at<2>().has_extension<cli_meta>());
}

TEST_CASE("field_descriptor via reflect_on — default value", "[reflect_on][descriptor][value]") {
    constexpr auto schema = reflect<RegWithExt>();

    // days defaults to 5
    auto v = schema.at<1>().value();
    REQUIRE(v == 5);
}

TEST_CASE("field_descriptor via reflect_on — default with()", "[reflect_on][descriptor][with]") {
    constexpr auto schema = reflect<RegWithExt>();

    auto w = schema.at<2>().with();
    REQUIRE(w.cli.short_flag == 'v');
}

// ── for_each with instance via registration ──────────────────────────────

TEST_CASE("for_each via reflect_on iterates field<> members", "[reflect_on][for_each]") {
    RegSimple args;
    args.name = "Alice";
    args.age = 30;
    args.active = true;

    std::vector<std::string> collected;
    reflect<RegSimple>().for_each(args, [&](auto field) {
        collected.emplace_back(field.name());
    });

    REQUIRE(collected.size() == 3);
    REQUIRE(collected[0] == "name");
    REQUIRE(collected[1] == "age");
    REQUIRE(collected[2] == "active");
}

TEST_CASE("for_each via reflect_on — mutable value access", "[reflect_on][for_each]") {
    RegSimple args;
    args.name = "Bob";

    reflect<RegSimple>().for_each(args, [](auto field) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field.value())>, std::string>) {
            field.value() = "Changed";
        }
    });

    REQUIRE(args.name.value == "Changed");
}

TEST_CASE("for_each via reflect_on skips non-field<> members", "[reflect_on][for_each]") {
    RegMixed ms;
    ms.label = "hello";
    ms.counter = 999;
    ms.score = 42;

    std::vector<std::string> names;
    reflect<RegMixed>().for_each(ms, [&](auto field) {
        names.emplace_back(field.name());
    });

    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "label");
    REQUIRE(names[1] == "score");
}

TEST_CASE("for_each via reflect_on — extension access", "[reflect_on][for_each][extension]") {
    RegWithExt args;

    char found_flag = '\0';
    reflect<RegWithExt>().for_each(args, [&](auto field) {
        if constexpr (field.template has_extension<cli_meta>()) {
            found_flag = field.with().cli.short_flag;
        }
    });

    REQUIRE(found_flag == 'v');
}

// ── get by runtime name via registration ─────────────────────────────────

TEST_CASE("get via reflect_on finds field by name", "[reflect_on][get]") {
    RegSimple args;
    args.age = 25;

    int found_value = 0;
    bool found = reflect<RegSimple>().get(args, "age", [&](auto field) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field.value())>, int>) {
            found_value = field.value();
        }
    });

    REQUIRE(found);
    REQUIRE(found_value == 25);
}

TEST_CASE("get via reflect_on returns false for unknown name", "[reflect_on][get]") {
    RegSimple args;

    bool found = reflect<RegSimple>().get(args, "nonexistent", [](auto) {});

    REQUIRE(!found);
}

// ── reflect(instance) deduction with registered type ─────────────────────

TEST_CASE("reflect(instance) with registered type", "[reflect_on]") {
    const RegSimple args{};

    auto schema = reflect(args);
    REQUIRE(schema.name() == "RegSimple");
    REQUIRE(schema.count() == 3);
}

// ── reflected_struct concept with registered types ───────────────────────

TEST_CASE("reflected_struct accepts registered types", "[reflect_on][concept]") {
    STATIC_REQUIRE(reflected_struct<RegSimple>);
    STATIC_REQUIRE(reflected_struct<RegMixed>);
    STATIC_REQUIRE(reflected_struct<RegWithExt>);
    STATIC_REQUIRE(reflected_struct<RegSingle>);
}

// ── Single-field edge case ───────────────────────────────────────────────

TEST_CASE("reflect_on with single field", "[reflect_on][edge]") {
    RegSingle s;
    s.value = 42;

    REQUIRE(field_count<RegSingle>() == 1);
    REQUIRE(field_name<0, RegSingle>() == "value");

    int got = 0;
    reflect<RegSingle>().for_each(s, [&](auto field) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field.value())>, int>) {
            got = field.value();
        }
    });
    REQUIRE(got == 42);
}

// ── JSON round-trip via registration ─────────────────────────────────────

TEST_CASE("to_json via reflect_on", "[reflect_on][json]") {
    RegSimple args;
    args.name = "Charlie";
    args.age = 40;
    args.active = true;

    auto j = to_json(args);
    REQUIRE(j["name"] == "Charlie");
    REQUIRE(j["age"] == 40);
    REQUIRE(j["active"] == true);
}

TEST_CASE("to_json via reflect_on — mixed struct", "[reflect_on][json]") {
    RegMixed ms;
    ms.label = "test";
    ms.counter = 999;
    ms.score = 77;

    auto j = to_json(ms);
    REQUIRE(j["label"] == "test");
    REQUIRE(j["score"] == 77);
    // "counter" is not a field<>, should not appear
    REQUIRE(!j.contains("counter"));
}

TEST_CASE("to_json_string via reflect_on", "[reflect_on][json]") {
    RegSingle s;
    s.value = 99;

    auto compact = to_json_string(s);
    REQUIRE(compact == R"({"value":99})");
}
