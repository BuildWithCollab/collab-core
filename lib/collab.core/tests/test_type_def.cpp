#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

import collab.core;

using namespace collab::model;

// ═════════════════════════════════════════════════════════════════════════
// Test metadata types
// ═════════════════════════════════════════════════════════════════════════

struct endpoint_info {
    const char* path   = "";
    const char* method = "GET";
};

struct help_info {
    const char* summary = "";
};

struct tag_info {
    const char* value = "";
};

struct cli_meta {
    struct { char short_flag = '\0'; } cli;
};

struct render_meta {
    struct { const char* style = ""; int width = 0; } render;
};

// ═════════════════════════════════════════════════════════════════════════
// Test structs
// ═════════════════════════════════════════════════════════════════════════

// ── Simple: fields only, no meta ─────────────────────────────────────────

struct SimpleArgs {
    field<std::string> name;
    field<int>         age;
    field<bool>        active;
};

// ── Fields + metas ───────────────────────────────────────────────────────

struct Dog {
    meta<endpoint_info> endpoint{{.path = "/dogs", .method = "POST"}};
    meta<help_info>     help{{.summary = "A good boy"}};

    field<std::string>  name;
    field<int>          age;
    field<std::string>  breed;
};

// ── Mixed: fields, metas, and plain members ──────────────────────────────

struct MixedStruct {
    meta<help_info>    help{{.summary = "mixed test"}};

    field<std::string> label;
    int                counter = 0;
    field<int>         score;
};

// ── Multiple metas of the same type ──────────────────────────────────────

struct MultiTagged {
    meta<tag_info> tag1{{.value = "pet"}};
    meta<tag_info> tag2{{.value = "animal"}};
    meta<tag_info> tag3{{.value = "good-boy"}};

    field<std::string> name;
};

// ── Meta only, no fields ─────────────────────────────────────────────────

struct MetaOnly {
    meta<endpoint_info> endpoint{{.path = "/health", .method = "GET"}};
    meta<help_info>     help{{.summary = "health check"}};
};

// ── Single field ─────────────────────────────────────────────────────────

struct SingleField {
    field<int> value;
};

// ── Field with extensions (with<>) ───────────────────────────────────────

struct CliArgs {
    meta<help_info> help{{.summary = "CLI tool"}};

    field<std::string>                      query;
    field<bool, with<cli_meta>>             verbose{.with = {{.cli = {.short_flag = 'v'}}}};
    field<int, with<cli_meta, render_meta>> limit{
        .with = {{.cli = {.short_flag = 'l'}}, {.render = {.style = "bold", .width = 10}}}
    };
};

// ═════════════════════════════════════════════════════════════════════════
// reflect_on fallbacks (non-PFR builds)
// ═════════════════════════════════════════════════════════════════════════

#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::model::reflect_on<SimpleArgs>() {
    return collab::model::field_info<SimpleArgs>("name", "age", "active");
}

template <>
constexpr auto collab::model::reflect_on<Dog>() {
    return collab::model::field_info<Dog>("endpoint", "help", "name", "age", "breed");
}

template <>
constexpr auto collab::model::reflect_on<MixedStruct>() {
    return collab::model::field_info<MixedStruct>("help", "label", "counter", "score");
}

template <>
constexpr auto collab::model::reflect_on<MultiTagged>() {
    return collab::model::field_info<MultiTagged>("tag1", "tag2", "tag3", "name");
}

template <>
constexpr auto collab::model::reflect_on<MetaOnly>() {
    return collab::model::field_info<MetaOnly>("endpoint", "help");
}

template <>
constexpr auto collab::model::reflect_on<SingleField>() {
    return collab::model::field_info<SingleField>("value");
}

template <>
constexpr auto collab::model::reflect_on<CliArgs>() {
    return collab::model::field_info<CliArgs>("help", "query", "verbose", "limit");
}
#endif

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::name()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def name() returns the struct type name", "[type_def][name]") {
    type_def<SimpleArgs> t;
    REQUIRE(t.name() == "SimpleArgs");
}

TEST_CASE("type_def name() works for various types", "[type_def][name]") {
    REQUIRE(type_def<Dog>{}.name() == "Dog");
    REQUIRE(type_def<MixedStruct>{}.name() == "MixedStruct");
    REQUIRE(type_def<SingleField>{}.name() == "SingleField");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::field_count()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def field_count() counts only field<> members", "[type_def][field_count]") {
    REQUIRE(type_def<SimpleArgs>{}.field_count() == 3);
    REQUIRE(type_def<Dog>{}.field_count() == 3);
    REQUIRE(type_def<SingleField>{}.field_count() == 1);
}

TEST_CASE("type_def field_count() excludes meta<> members", "[type_def][field_count]") {
    // Dog has 2 metas + 3 fields = 5 total members, but only 3 fields
    REQUIRE(type_def<Dog>{}.field_count() == 3);
}

TEST_CASE("type_def field_count() excludes plain members", "[type_def][field_count]") {
    // MixedStruct has 1 meta + 1 field + 1 plain + 1 field = 4 total, 2 are field<>
    REQUIRE(type_def<MixedStruct>{}.field_count() == 2);
}

TEST_CASE("type_def field_count() is zero when struct has only metas", "[type_def][field_count]") {
    REQUIRE(type_def<MetaOnly>{}.field_count() == 0);
}

TEST_CASE("type_def field_count() for multi-tagged struct", "[type_def][field_count]") {
    // 3 metas + 1 field
    REQUIRE(type_def<MultiTagged>{}.field_count() == 1);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::field_names()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def field_names() returns field<> member names only", "[type_def][field_names]") {
    auto names = type_def<Dog>{}.field_names();
    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "breed");
}

TEST_CASE("type_def field_names() excludes meta and plain members", "[type_def][field_names]") {
    auto names = type_def<MixedStruct>{}.field_names();
    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "label");
    REQUIRE(names[1] == "score");
}

TEST_CASE("type_def field_names() for simple struct", "[type_def][field_names]") {
    auto names = type_def<SimpleArgs>{}.field_names();
    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "active");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::has_meta<M>()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def has_meta() detects present metas", "[type_def][has_meta]") {
    type_def<Dog> t;
    REQUIRE(t.has_meta<endpoint_info>());
    REQUIRE(t.has_meta<help_info>());
}

TEST_CASE("type_def has_meta() returns false for absent metas", "[type_def][has_meta]") {
    type_def<Dog> t;
    REQUIRE(!t.has_meta<tag_info>());
    REQUIRE(!t.has_meta<cli_meta>());
}

TEST_CASE("type_def has_meta() returns false when struct has no metas", "[type_def][has_meta]") {
    type_def<SimpleArgs> t;
    REQUIRE(!t.has_meta<endpoint_info>());
    REQUIRE(!t.has_meta<help_info>());
}

TEST_CASE("type_def has_meta() works for multi-tagged struct", "[type_def][has_meta]") {
    type_def<MultiTagged> t;
    REQUIRE(t.has_meta<tag_info>());
    REQUIRE(!t.has_meta<endpoint_info>());
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::meta<M>()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def meta() returns the metadata value", "[type_def][meta]") {
    type_def<Dog> t;
    auto ep = t.meta<endpoint_info>();
    REQUIRE(std::string_view{ep.path} == "/dogs");
    REQUIRE(std::string_view{ep.method} == "POST");
}

TEST_CASE("type_def meta() returns different metadata types", "[type_def][meta]") {
    type_def<Dog> t;
    auto h = t.meta<help_info>();
    REQUIRE(std::string_view{h.summary} == "A good boy");
}

TEST_CASE("type_def meta() returns first when multiple of same type", "[type_def][meta]") {
    type_def<MultiTagged> t;
    auto tag = t.meta<tag_info>();
    REQUIRE(std::string_view{tag.value} == "pet");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::meta_count<M>()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def meta_count() returns correct count", "[type_def][meta_count]") {
    REQUIRE(type_def<Dog>{}.meta_count<endpoint_info>() == 1);
    REQUIRE(type_def<Dog>{}.meta_count<help_info>() == 1);
    REQUIRE(type_def<Dog>{}.meta_count<tag_info>() == 0);
}

TEST_CASE("type_def meta_count() for multiple metas of same type", "[type_def][meta_count]") {
    REQUIRE(type_def<MultiTagged>{}.meta_count<tag_info>() == 3);
}

TEST_CASE("type_def meta_count() is zero for no-meta struct", "[type_def][meta_count]") {
    REQUIRE(type_def<SimpleArgs>{}.meta_count<endpoint_info>() == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::metas<M>()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def metas() returns all metas of a given type", "[type_def][metas]") {
    type_def<MultiTagged> t;
    auto tags = t.metas<tag_info>();
    REQUIRE(tags.size() == 3);
    REQUIRE(std::string_view{tags[0].value} == "pet");
    REQUIRE(std::string_view{tags[1].value} == "animal");
    REQUIRE(std::string_view{tags[2].value} == "good-boy");
}

TEST_CASE("type_def metas() returns single-element vector for one meta", "[type_def][metas]") {
    type_def<Dog> t;
    auto eps = t.metas<endpoint_info>();
    REQUIRE(eps.size() == 1);
    REQUIRE(std::string_view{eps[0].path} == "/dogs");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::for_each() — instance iteration
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def for_each iterates field<> members with name and value", "[type_def][for_each]") {
    SimpleArgs args;
    args.name = "Alice";
    args.age = 30;
    args.active = true;

    std::vector<std::string> collected_names;
    type_def<SimpleArgs>{}.for_each(args, [&](std::string_view name, auto& value) {
        collected_names.emplace_back(name);
    });

    REQUIRE(collected_names.size() == 3);
    REQUIRE(collected_names[0] == "name");
    REQUIRE(collected_names[1] == "age");
    REQUIRE(collected_names[2] == "active");
}

TEST_CASE("type_def for_each provides access to field values", "[type_def][for_each]") {
    SimpleArgs args;
    args.name = "Bob";
    args.age = 25;
    args.active = false;

    std::string found_name;
    int found_age = 0;

    type_def<SimpleArgs>{}.for_each(args, [&](std::string_view name, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
            if (name == "name") found_name = value;
        } else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
            if (name == "age") found_age = value;
        }
    });

    REQUIRE(found_name == "Bob");
    REQUIRE(found_age == 25);
}

TEST_CASE("type_def for_each provides mutable value access", "[type_def][for_each]") {
    SimpleArgs args;
    args.name = "Original";

    type_def<SimpleArgs>{}.for_each(args, [](std::string_view name, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
            value = "Changed";
        }
    });

    REQUIRE(args.name.value == "Changed");
}

TEST_CASE("type_def for_each skips meta<> members", "[type_def][for_each]") {
    Dog rex;
    rex.name = "Rex";
    rex.age = 3;
    rex.breed = "Husky";

    std::vector<std::string> names;
    type_def<Dog>{}.for_each(rex, [&](std::string_view name, auto&) {
        names.emplace_back(name);
    });

    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "breed");
    // "endpoint" and "help" (metas) should NOT appear
}

TEST_CASE("type_def for_each skips plain members", "[type_def][for_each]") {
    MixedStruct ms;
    ms.label = "hello";
    ms.counter = 999;
    ms.score = 42;

    std::vector<std::string> names;
    type_def<MixedStruct>{}.for_each(ms, [&](std::string_view name, auto&) {
        names.emplace_back(name);
    });

    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "label");
    REQUIRE(names[1] == "score");
    // "counter" (plain int) and "help" (meta) should NOT appear
}

TEST_CASE("type_def for_each on meta-only struct calls nothing", "[type_def][for_each]") {
    MetaOnly mo;

    int count = 0;
    type_def<MetaOnly>{}.for_each(mo, [&](std::string_view, auto&) {
        ++count;
    });

    REQUIRE(count == 0);
}

TEST_CASE("type_def for_each with const instance", "[type_def][for_each][const]") {
    SimpleArgs args;
    args.name = "Const";
    args.age = 99;
    args.active = true;
    const SimpleArgs& cargs = args;

    std::vector<std::string> names;
    type_def<SimpleArgs>{}.for_each(cargs, [&](std::string_view name, const auto&) {
        names.emplace_back(name);
    });

    REQUIRE(names.size() == 3);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::for_each_field() — schema-only iteration
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def for_each_field iterates field descriptors", "[type_def][for_each_field]") {
    std::vector<std::string> names;
    std::vector<std::size_t> indices;

    type_def<Dog>{}.for_each_field([&](auto descriptor) {
        names.emplace_back(descriptor.name());
        if constexpr (requires { descriptor.index(); }) {
            indices.push_back(descriptor.index());
        }
    });

    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "name");
    REQUIRE(names[1] == "age");
    REQUIRE(names[2] == "breed");
    // Indices are raw struct member indices (metas occupy 0 and 1)
    REQUIRE(indices[0] == 2);
    REQUIRE(indices[1] == 3);
    REQUIRE(indices[2] == 4);
}

TEST_CASE("type_def for_each_field skips meta members", "[type_def][for_each_field]") {
    std::vector<std::string> names;
    type_def<MixedStruct>{}.for_each_field([&](auto descriptor) {
        names.emplace_back(descriptor.name());
    });

    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "label");
    REQUIRE(names[1] == "score");
}

TEST_CASE("type_def for_each_field on meta-only struct yields nothing", "[type_def][for_each_field]") {
    int count = 0;
    type_def<MetaOnly>{}.for_each_field([&](auto) {
        ++count;
    });
    REQUIRE(count == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::for_each_meta() — meta iteration
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def for_each_meta iterates meta values (schema-only)", "[type_def][for_each_meta]") {
    int meta_count = 0;
    type_def<Dog>{}.for_each_meta([&](auto& meta_value) {
        ++meta_count;
    });
    REQUIRE(meta_count == 2);
}

TEST_CASE("type_def for_each_meta provides correct values", "[type_def][for_each_meta]") {
    bool found_endpoint = false;
    bool found_help = false;

    type_def<Dog>{}.for_each_meta([&](auto& meta_value) {
        using M = std::remove_cvref_t<decltype(meta_value)>;
        if constexpr (std::is_same_v<M, endpoint_info>) {
            REQUIRE(std::string_view{meta_value.path} == "/dogs");
            found_endpoint = true;
        } else if constexpr (std::is_same_v<M, help_info>) {
            REQUIRE(std::string_view{meta_value.summary} == "A good boy");
            found_help = true;
        }
    });

    REQUIRE(found_endpoint);
    REQUIRE(found_help);
}

TEST_CASE("type_def for_each_meta on struct with no metas", "[type_def][for_each_meta]") {
    int count = 0;
    type_def<SimpleArgs>{}.for_each_meta([&](auto&) {
        ++count;
    });
    REQUIRE(count == 0);
}

TEST_CASE("type_def for_each_meta with multiple metas of same type", "[type_def][for_each_meta]") {
    std::vector<std::string> tag_values;
    type_def<MultiTagged>{}.for_each_meta([&](auto& meta_value) {
        using M = std::remove_cvref_t<decltype(meta_value)>;
        if constexpr (std::is_same_v<M, tag_info>) {
            tag_values.emplace_back(meta_value.value);
        }
    });

    REQUIRE(tag_values.size() == 3);
    REQUIRE(tag_values[0] == "pet");
    REQUIRE(tag_values[1] == "animal");
    REQUIRE(tag_values[2] == "good-boy");
}

TEST_CASE("type_def for_each_meta with instance reads from that instance", "[type_def][for_each_meta]") {
    Dog rex;
    // Default-constructed Dog has the struct-defined meta values
    bool found = false;
    type_def<Dog>{}.for_each_meta(rex, [&](auto& meta_value) {
        using M = std::remove_cvref_t<decltype(meta_value)>;
        if constexpr (std::is_same_v<M, endpoint_info>) {
            REQUIRE(std::string_view{meta_value.path} == "/dogs");
            found = true;
        }
    });
    REQUIRE(found);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::get() — find field by runtime name
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def get() finds field by name", "[type_def][get]") {
    SimpleArgs args;
    args.age = 25;

    int found_value = 0;
    bool found = type_def<SimpleArgs>{}.get(args, "age", [&](std::string_view, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
            found_value = value;
        }
    });

    REQUIRE(found);
    REQUIRE(found_value == 25);
}

TEST_CASE("type_def get() returns false for unknown name", "[type_def][get]") {
    SimpleArgs args;

    bool found = type_def<SimpleArgs>{}.get(args, "nonexistent", [](std::string_view, auto&) {});
    REQUIRE(!found);
}

TEST_CASE("type_def get() returns false for meta member names", "[type_def][get]") {
    Dog rex;

    bool found = type_def<Dog>{}.get(rex, "endpoint", [](std::string_view, auto&) {});
    REQUIRE(!found);

    found = type_def<Dog>{}.get(rex, "help", [](std::string_view, auto&) {});
    REQUIRE(!found);
}

TEST_CASE("type_def get() allows mutation", "[type_def][get]") {
    SimpleArgs args;
    args.name = "Original";

    type_def<SimpleArgs>{}.get(args, "name", [](std::string_view, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
            value = "Modified";
        }
    });

    REQUIRE(args.name.value == "Modified");
}

TEST_CASE("type_def get() on const instance", "[type_def][get]") {
    SimpleArgs args;
    args.age = 42;
    const SimpleArgs& cargs = args;

    int found_value = 0;
    bool found = type_def<SimpleArgs>{}.get(cargs, "age", [&](std::string_view, const auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
            found_value = value;
        }
    });

    REQUIRE(found);
    REQUIRE(found_value == 42);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Field-level metadata via with<>
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def for_each_field detects field-level metas", "[type_def][field_meta]") {
    bool found_verbose_cli = false;
    bool found_limit_cli = false;
    bool found_limit_render = false;

    type_def<CliArgs>{}.for_each_field([&](auto descriptor) {
        if (descriptor.name() == "verbose") {
            found_verbose_cli = descriptor.template has_meta<cli_meta>();
        }
        if (descriptor.name() == "limit") {
            found_limit_cli = descriptor.template has_meta<cli_meta>();
            found_limit_render = descriptor.template has_meta<render_meta>();
        }
    });

    REQUIRE(found_verbose_cli);
    REQUIRE(found_limit_cli);
    REQUIRE(found_limit_render);
}

TEST_CASE("type_def for_each_field reads meta values via meta<M>()", "[type_def][field_meta]") {
    char verbose_flag = '\0';

    type_def<CliArgs>{}.for_each_field([&](auto descriptor) {
        if constexpr (requires { descriptor.index(); }) {
            if (descriptor.name() == "verbose") {
                if constexpr (descriptor.template has_meta<cli_meta>()) {
                    verbose_flag = descriptor.template meta<cli_meta>().cli.short_flag;
                }
            }
        }
    });

    REQUIRE(verbose_flag == 'v');
}

TEST_CASE("type_def for_each_field — query has no meta", "[type_def][field_meta]") {
    bool query_has_cli = true;

    type_def<CliArgs>{}.for_each_field([&](auto descriptor) {
        if (descriptor.name() == "query") {
            query_has_cli = descriptor.template has_meta<cli_meta>();
        }
    });

    REQUIRE(!query_has_cli);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Edge cases
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def with single field struct", "[type_def][edge]") {
    type_def<SingleField> t;
    REQUIRE(t.field_count() == 1);
    REQUIRE(t.field_names()[0] == "value");

    SingleField s;
    s.value = 42;

    int got = 0;
    t.for_each(s, [&](std::string_view name, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
            got = value;
        }
    });
    REQUIRE(got == 42);
}

TEST_CASE("type_def with meta-only struct has zero fields", "[type_def][edge]") {
    type_def<MetaOnly> t;
    REQUIRE(t.field_count() == 0);
    REQUIRE(t.has_meta<endpoint_info>());
    REQUIRE(t.has_meta<help_info>());

    auto ep = t.meta<endpoint_info>();
    REQUIRE(std::string_view{ep.path} == "/health");
}

TEST_CASE("type_def default field values are accessible", "[type_def][edge]") {
    Dog rex;  // default constructed

    std::string breed_value;
    type_def<Dog>{}.for_each(rex, [&](std::string_view name, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
            if (name == "breed") breed_value = value;
        }
    });

    // Default std::string is empty
    REQUIRE(breed_value.empty());
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T> is stateless — multiple instances are equivalent
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def is stateless — two instances behave identically", "[type_def][stateless]") {
    type_def<Dog> t1;
    type_def<Dog> t2;

    REQUIRE(t1.name() == t2.name());
    REQUIRE(t1.field_count() == t2.field_count());
    REQUIRE(t1.has_meta<endpoint_info>() == t2.has_meta<endpoint_info>());

    auto ep1 = t1.meta<endpoint_info>();
    auto ep2 = t2.meta<endpoint_info>();
    REQUIRE(std::string_view{ep1.path} == std::string_view{ep2.path});
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def with CliArgs — full integration
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def CliArgs integration — fields, metas, and extensions", "[type_def][integration]") {
    type_def<CliArgs> t;

    // Schema
    REQUIRE(t.name() == "CliArgs");
    REQUIRE(t.field_count() == 3);
    REQUIRE(t.has_meta<help_info>());

    auto h = t.meta<help_info>();
    REQUIRE(std::string_view{h.summary} == "CLI tool");

    // Field names
    auto names = t.field_names();
    REQUIRE(names[0] == "query");
    REQUIRE(names[1] == "verbose");
    REQUIRE(names[2] == "limit");

    // Instance iteration
    CliArgs args;
    args.query = "hello world";
    args.verbose = true;
    args.limit = 50;

    std::string query_val;
    type_def<CliArgs>{}.for_each(args, [&](std::string_view name, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
            if (name == "query") query_val = value;
        }
    });
    REQUIRE(query_val == "hello world");

    // Get by name
    bool found = t.get(args, "verbose", [](std::string_view, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, bool>) {
            REQUIRE(value == true);
        }
    });
    REQUIRE(found);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::set() — assign field by runtime name
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def set() assigns a string field", "[type_def][set]") {
    SimpleArgs args;

    bool ok = type_def<SimpleArgs>{}.set(args, "name", std::string("Alice"));
    REQUIRE(ok);
    REQUIRE(args.name.value == "Alice");
}

TEST_CASE("type_def set() assigns an int field", "[type_def][set]") {
    SimpleArgs args;

    bool ok = type_def<SimpleArgs>{}.set(args, "age", 30);
    REQUIRE(ok);
    REQUIRE(args.age.value == 30);
}

TEST_CASE("type_def set() assigns a bool field", "[type_def][set]") {
    SimpleArgs args;

    bool ok = type_def<SimpleArgs>{}.set(args, "active", true);
    REQUIRE(ok);
    REQUIRE(args.active.value == true);
}

TEST_CASE("type_def set() returns false for unknown field name", "[type_def][set]") {
    SimpleArgs args;

    bool ok = type_def<SimpleArgs>{}.set(args, "nonexistent", 42);
    REQUIRE(!ok);
}

TEST_CASE("type_def set() returns false for type mismatch", "[type_def][set]") {
    SimpleArgs args;

    // "name" is field<std::string>, passing an int
    bool ok = type_def<SimpleArgs>{}.set(args, "name", 42);
    REQUIRE(!ok);
}

TEST_CASE("type_def set() does not modify field on type mismatch", "[type_def][set]") {
    SimpleArgs args;
    args.name = "Original";

    type_def<SimpleArgs>{}.set(args, "name", 42);
    REQUIRE(args.name.value == "Original");
}

TEST_CASE("type_def set() ignores meta member names", "[type_def][set]") {
    Dog rex;

    bool ok = type_def<Dog>{}.set(rex, "endpoint", 42);
    REQUIRE(!ok);
}

TEST_CASE("type_def set() works on struct with metas", "[type_def][set]") {
    Dog rex;

    REQUIRE(type_def<Dog>{}.set(rex, "name", std::string("Rex")));
    REQUIRE(type_def<Dog>{}.set(rex, "age", 3));
    REQUIRE(type_def<Dog>{}.set(rex, "breed", std::string("Husky")));

    REQUIRE(rex.name.value == "Rex");
    REQUIRE(rex.age.value == 3);
    REQUIRE(rex.breed.value == "Husky");
}

TEST_CASE("type_def set() overwrites existing values", "[type_def][set]") {
    SimpleArgs args;
    args.name = "First";

    type_def<SimpleArgs>{}.set(args, "name", std::string("Second"));
    REQUIRE(args.name.value == "Second");
}

TEST_CASE("type_def set() with const char* to string field", "[type_def][set]") {
    SimpleArgs args;

    bool ok = type_def<SimpleArgs>{}.set(args, "name", "hello");
    REQUIRE(ok);
    REQUIRE(args.name.value == "hello");
}

TEST_CASE("type_def set() round-trips with get()", "[type_def][set]") {
    Dog rex;
    type_def<Dog> t;

    t.set(rex, "name", std::string("Buddy"));
    t.set(rex, "age", 7);

    std::string got_name;
    int got_age = 0;
    t.get(rex, "name", [&](std::string_view, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
            got_name = value;
        }
    });
    t.get(rex, "age", [&](std::string_view, auto& value) {
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
            got_age = value;
        }
    });

    REQUIRE(got_name == "Buddy");
    REQUIRE(got_age == 7);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::has_field()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def has_field() finds field<> members by name", "[type_def][has_field]") {
    type_def<Dog> t;
    REQUIRE(t.has_field("name"));
    REQUIRE(t.has_field("age"));
    REQUIRE(t.has_field("breed"));
}

TEST_CASE("type_def has_field() returns false for unknown names", "[type_def][has_field]") {
    type_def<Dog> t;
    REQUIRE(!t.has_field("nope"));
    REQUIRE(!t.has_field(""));
    REQUIRE(!t.has_field("Name"));
}

TEST_CASE("type_def has_field() returns false for meta member names", "[type_def][has_field]") {
    type_def<Dog> t;
    REQUIRE(!t.has_field("endpoint"));
    REQUIRE(!t.has_field("help"));
}

TEST_CASE("type_def has_field() returns false for plain member names", "[type_def][has_field]") {
    type_def<MixedStruct> t;
    REQUIRE(t.has_field("label"));
    REQUIRE(t.has_field("score"));
    REQUIRE(!t.has_field("counter"));
}

TEST_CASE("type_def has_field() on single-field struct", "[type_def][has_field]") {
    type_def<SingleField> t;
    REQUIRE(t.has_field("value"));
    REQUIRE(!t.has_field("other"));
}

TEST_CASE("type_def has_field() on meta-only struct", "[type_def][has_field]") {
    type_def<MetaOnly> t;
    REQUIRE(!t.has_field("endpoint"));
    REQUIRE(!t.has_field("help"));
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::field() — field query by runtime name
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def field() returns view with correct name", "[type_def][field_query]") {
    type_def<SimpleArgs> t;
    auto fv = t.field("name");
    REQUIRE(fv.name() == "name");
}

TEST_CASE("type_def field() works for each field", "[type_def][field_query]") {
    type_def<Dog> t;
    REQUIRE(t.field("name").name() == "name");
    REQUIRE(t.field("age").name() == "age");
    REQUIRE(t.field("breed").name() == "breed");
}

TEST_CASE("type_def field() for unknown name returns view with that name", "[type_def][field_query]") {
    type_def<Dog> t;
    auto fv = t.field("nonexistent");
    REQUIRE(fv.name() == "nonexistent");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::get<V>() — optional-returning overload
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def get<V>() returns value for matching type", "[type_def][get_typed]") {
    SimpleArgs args;
    args.name = "Alice";
    args.age = 30;

    type_def<SimpleArgs> t;
    REQUIRE(t.get<std::string>(args, "name") == "Alice");
    REQUIRE(t.get<int>(args, "age") == 30);
}

TEST_CASE("type_def get<V>() returns nullopt for unknown field", "[type_def][get_typed]") {
    SimpleArgs args;
    type_def<SimpleArgs> t;
    REQUIRE(!t.get<int>(args, "nope").has_value());
}

TEST_CASE("type_def get<V>() returns nullopt for type mismatch", "[type_def][get_typed]") {
    SimpleArgs args;
    args.age = 42;
    type_def<SimpleArgs> t;
    REQUIRE(!t.get<std::string>(args, "age").has_value());
}

TEST_CASE("type_def get<V>() round-trips with set()", "[type_def][get_typed]") {
    Dog rex;
    type_def<Dog> t;

    t.set(rex, "name", std::string("Rex"));
    t.set(rex, "age", 5);
    t.set(rex, "breed", std::string("Husky"));

    REQUIRE(t.get<std::string>(rex, "name") == "Rex");
    REQUIRE(t.get<int>(rex, "age") == 5);
    REQUIRE(t.get<std::string>(rex, "breed") == "Husky");
}

TEST_CASE("type_def get<V>() on const instance", "[type_def][get_typed]") {
    SimpleArgs args;
    args.active = true;
    const SimpleArgs& cargs = args;

    type_def<SimpleArgs> t;
    REQUIRE(t.get<bool>(cargs, "active") == true);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: type_def<T>::create()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("type_def create() returns default-constructed instance", "[type_def][create]") {
    type_def<SimpleArgs> t;
    SimpleArgs args = t.create();
    REQUIRE(args.name.value.empty());
    REQUIRE(args.age.value == 0);
    REQUIRE(args.active.value == false);
}

TEST_CASE("type_def create() preserves field defaults", "[type_def][create]") {
    type_def<Dog> t;
    Dog d = t.create();
    REQUIRE(d.name.value.empty());
    REQUIRE(d.age.value == 0);
    REQUIRE(d.breed.value.empty());
}

TEST_CASE("type_def create() result is mutable and works with set", "[type_def][create]") {
    type_def<Dog> t;
    Dog d = t.create();
    t.set(d, "name", std::string("Buddy"));
    REQUIRE(t.get<std::string>(d, "name") == "Buddy");
}
