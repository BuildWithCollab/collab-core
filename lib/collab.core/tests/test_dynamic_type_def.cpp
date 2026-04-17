#include <catch2/catch_test_macros.hpp>

#include <any>
#include <string>
#include <string_view>
#include <vector>

import collab.core;

using namespace collab::model;

// ═════════════════════════════════════════════════════════════════════════
// Test metadata types (same as test_type_def.cpp — shared vocabulary)
// ═════════════════════════════════════════════════════════════════════════

struct endpoint_info_d {
    const char* path   = "";
    const char* method = "GET";
};

struct help_info_d {
    const char* summary = "";
};

struct tag_info_d {
    const char* value = "";
};

struct cli_meta_d {
    struct { char short_flag = '\0'; } cli;
};

struct render_meta_d {
    struct { const char* style = ""; int width = 0; } render;
};

// ═════════════════════════════════════════════════════════════════════════
// Tests: Construction and name()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def name()", "[type_def][dynamic]") {
    auto t = type_def("Event");
    REQUIRE(t.name() == "Event");
}

TEST_CASE("dynamic type_def name() with different names", "[type_def][dynamic]") {
    REQUIRE(type_def("Dog").name() == "Dog");
    REQUIRE(type_def("Widget").name() == "Widget");
    REQUIRE(type_def("").name() == "");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: field_count()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def field_count() with no fields", "[type_def][dynamic][field_count]") {
    auto t = type_def("Empty");
    REQUIRE(t.field_count() == 0);
}

TEST_CASE("dynamic type_def field_count()", "[type_def][dynamic][field_count]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count");
    REQUIRE(t.field_count() == 2);
}

TEST_CASE("dynamic type_def field_count() ignores type-level metas", "[type_def][dynamic][field_count]") {
    auto t = type_def("Event")
        .meta<endpoint_info_d>({.path = "/events"})
        .field<std::string>("title");
    REQUIRE(t.field_count() == 1);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: field_names()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def field_names()", "[type_def][dynamic][field_names]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("attendees")
        .field<bool>("verbose");
    auto names = t.field_names();
    REQUIRE(names.size() == 3);
    REQUIRE(names[0] == "title");
    REQUIRE(names[1] == "attendees");
    REQUIRE(names[2] == "verbose");
}

TEST_CASE("dynamic type_def field_names() empty", "[type_def][dynamic][field_names]") {
    auto t = type_def("Empty");
    REQUIRE(t.field_names().empty());
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: has_field()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def has_field()", "[type_def][dynamic][has_field]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count");
    REQUIRE(t.has_field("title"));
    REQUIRE(t.has_field("count"));
    REQUIRE(!t.has_field("nope"));
    REQUIRE(!t.has_field(""));
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: field() queries — name, defaults
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def field().name()", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    REQUIRE(t.field("title").name() == "title");
}

TEST_CASE("dynamic type_def field().has_default() false", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    REQUIRE(!t.field("title").has_default());
}

TEST_CASE("dynamic type_def field().has_default() true", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<int>("count", 100);
    REQUIRE(t.field("count").has_default());
}

TEST_CASE("dynamic type_def field().default_value<T>()", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<int>("count", 100)
        .field<std::string>("title", std::string("Untitled"));
    REQUIRE(t.field("count").default_value<int>() == 100);
    REQUIRE(t.field("title").default_value<std::string>() == "Untitled");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: field() queries — field-level metas via with<>
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def field with single meta", "[type_def][dynamic][field_meta]") {
    auto t = type_def("CLI")
        .field<bool>("verbose", false,
            with<cli_meta_d>({.cli = {.short_flag = 'v'}}));
    REQUIRE(t.field("verbose").has_meta<cli_meta_d>());
    REQUIRE(t.field("verbose").meta<cli_meta_d>().cli.short_flag == 'v');
}

TEST_CASE("dynamic type_def field with multiple metas", "[type_def][dynamic][field_meta]") {
    auto t = type_def("CLI")
        .field<bool>("verbose", false,
            with<cli_meta_d>({.cli = {.short_flag = 'v'}}),
            with<render_meta_d>({.render = {.style = "dim", .width = 5}}));
    REQUIRE(t.field("verbose").has_meta<cli_meta_d>());
    REQUIRE(t.field("verbose").has_meta<render_meta_d>());
    REQUIRE(t.field("verbose").meta<cli_meta_d>().cli.short_flag == 'v');
    REQUIRE(std::string_view{t.field("verbose").meta<render_meta_d>().render.style} == "dim");
    REQUIRE(t.field("verbose").meta<render_meta_d>().render.width == 5);
}

TEST_CASE("dynamic type_def field without meta returns false", "[type_def][dynamic][field_meta]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    REQUIRE(!t.field("title").has_meta<cli_meta_d>());
}

TEST_CASE("dynamic type_def field meta_count and metas", "[type_def][dynamic][field_meta]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string(""),
            with<tag_info_d>({.value = "a"}),
            with<tag_info_d>({.value = "b"}));
    REQUIRE(t.field("title").meta_count<tag_info_d>() == 2);
    auto tags = t.field("title").metas<tag_info_d>();
    REQUIRE(tags.size() == 2);
    REQUIRE(std::string_view{tags[0].value} == "a");
    REQUIRE(std::string_view{tags[1].value} == "b");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Type-level meta queries
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def has_meta()", "[type_def][dynamic][has_meta]") {
    auto t = type_def("Event")
        .meta<endpoint_info_d>({.path = "/events"});
    REQUIRE(t.has_meta<endpoint_info_d>());
    REQUIRE(!t.has_meta<help_info_d>());
}

TEST_CASE("dynamic type_def meta()", "[type_def][dynamic][meta]") {
    auto t = type_def("Event")
        .meta<endpoint_info_d>({.path = "/events", .method = "POST"});
    auto ep = t.meta<endpoint_info_d>();
    REQUIRE(std::string_view{ep.path} == "/events");
    REQUIRE(std::string_view{ep.method} == "POST");
}

TEST_CASE("dynamic type_def meta_count()", "[type_def][dynamic][meta_count]") {
    auto t = type_def("Event")
        .meta<tag_info_d>({.value = "a"})
        .meta<tag_info_d>({.value = "b"})
        .meta<tag_info_d>({.value = "c"});
    REQUIRE(t.meta_count<tag_info_d>() == 3);
    REQUIRE(t.meta_count<endpoint_info_d>() == 0);
}

TEST_CASE("dynamic type_def metas()", "[type_def][dynamic][metas]") {
    auto t = type_def("Event")
        .meta<tag_info_d>({.value = "a"})
        .meta<tag_info_d>({.value = "b"});
    auto tags = t.metas<tag_info_d>();
    REQUIRE(tags.size() == 2);
    REQUIRE(std::string_view{tags[0].value} == "a");
    REQUIRE(std::string_view{tags[1].value} == "b");
}

TEST_CASE("dynamic type_def meta() returns first when multiple", "[type_def][dynamic][meta]") {
    auto t = type_def("Event")
        .meta<tag_info_d>({.value = "first"})
        .meta<tag_info_d>({.value = "second"});
    REQUIRE(std::string_view{t.meta<tag_info_d>().value} == "first");
}

TEST_CASE("dynamic type_def has_meta() false when no metas", "[type_def][dynamic][has_meta]") {
    auto t = type_def("Simple")
        .field<int>("x");
    REQUIRE(!t.has_meta<endpoint_info_d>());
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: for_each_field()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def for_each_field()", "[type_def][dynamic][for_each_field]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count", 100);
    std::vector<std::string> names;
    bool found_default = false;
    t.for_each_field([&](field_view fd) {
        names.emplace_back(fd.name());
        if (fd.name() == "count" && fd.has_default())
            found_default = true;
    });
    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "title");
    REQUIRE(names[1] == "count");
    REQUIRE(found_default);
}

TEST_CASE("dynamic type_def for_each_field() empty", "[type_def][dynamic][for_each_field]") {
    auto t = type_def("Empty");
    int count = 0;
    t.for_each_field([&](field_view) { ++count; });
    REQUIRE(count == 0);
}

TEST_CASE("dynamic type_def for_each_field() can query field metas", "[type_def][dynamic][for_each_field]") {
    auto t = type_def("CLI")
        .field<std::string>("query")
        .field<bool>("verbose", false,
            with<cli_meta_d>({.cli = {.short_flag = 'v'}}));

    bool query_has_cli = true;
    bool verbose_has_cli = false;
    char verbose_flag = '\0';

    t.for_each_field([&](field_view fd) {
        if (fd.name() == "query")
            query_has_cli = fd.has_meta<cli_meta_d>();
        if (fd.name() == "verbose") {
            verbose_has_cli = fd.has_meta<cli_meta_d>();
            if (verbose_has_cli)
                verbose_flag = fd.meta<cli_meta_d>().cli.short_flag;
        }
    });

    REQUIRE(!query_has_cli);
    REQUIRE(verbose_has_cli);
    REQUIRE(verbose_flag == 'v');
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: for_each_meta()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def for_each_meta() iterates type-level metas", "[type_def][dynamic][for_each_meta]") {
    auto t = type_def("Event")
        .meta<endpoint_info_d>({.path = "/events"})
        .meta<help_info_d>({.summary = "An event"});
    int count = 0;
    t.for_each_meta([&](const std::any&) { ++count; });
    REQUIRE(count == 2);
}

TEST_CASE("dynamic type_def for_each_meta() on type with no metas", "[type_def][dynamic][for_each_meta]") {
    auto t = type_def("Simple")
        .field<int>("x");
    int count = 0;
    t.for_each_meta([&](const std::any&) { ++count; });
    REQUIRE(count == 0);
}

TEST_CASE("dynamic type_def for_each_meta() provides access via any_cast", "[type_def][dynamic][for_each_meta]") {
    auto t = type_def("Event")
        .meta<endpoint_info_d>({.path = "/events", .method = "POST"})
        .meta<help_info_d>({.summary = "An event"});

    bool found_endpoint = false;
    bool found_help = false;
    t.for_each_meta([&](const std::any& meta_value) {
        if (auto* ep = std::any_cast<endpoint_info_d>(&meta_value)) {
            REQUIRE(std::string_view{ep->path} == "/events");
            REQUIRE(std::string_view{ep->method} == "POST");
            found_endpoint = true;
        }
        if (auto* h = std::any_cast<help_info_d>(&meta_value)) {
            REQUIRE(std::string_view{h->summary} == "An event");
            found_help = true;
        }
    });

    REQUIRE(found_endpoint);
    REQUIRE(found_help);
}

TEST_CASE("dynamic type_def for_each_meta() with multiple metas of same type", "[type_def][dynamic][for_each_meta]") {
    auto t = type_def("Tagged")
        .meta<tag_info_d>({.value = "a"})
        .meta<tag_info_d>({.value = "b"})
        .meta<tag_info_d>({.value = "c"});

    std::vector<std::string> values;
    t.for_each_meta([&](const std::any& meta_value) {
        if (auto* tag = std::any_cast<tag_info_d>(&meta_value))
            values.emplace_back(tag->value);
    });

    REQUIRE(values.size() == 3);
    REQUIRE(values[0] == "a");
    REQUIRE(values[1] == "b");
    REQUIRE(values[2] == "c");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Full integration
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic type_def full integration", "[type_def][dynamic][integration]") {
    auto event_t = type_def("Event")
        .meta<endpoint_info_d>({.path = "/events"})
        .meta<help_info_d>({.summary = "An event"})
        .field<std::string>("title")
        .field<int>("attendees", 100,
            with<render_meta_d>({.render = {.style = "bold", .width = 10}}))
        .field<bool>("verbose", false,
            with<cli_meta_d>({.cli = {.short_flag = 'v'}}),
            with<render_meta_d>({.render = {.style = "dim"}}));

    // Schema
    REQUIRE(event_t.name() == "Event");
    REQUIRE(event_t.field_count() == 3);
    auto names = event_t.field_names();
    REQUIRE(names[0] == "title");
    REQUIRE(names[1] == "attendees");
    REQUIRE(names[2] == "verbose");

    // Type-level metas
    REQUIRE(event_t.has_meta<endpoint_info_d>());
    REQUIRE(std::string_view{event_t.meta<endpoint_info_d>().path} == "/events");
    REQUIRE(event_t.has_meta<help_info_d>());
    REQUIRE(std::string_view{event_t.meta<help_info_d>().summary} == "An event");
    REQUIRE(!event_t.has_meta<tag_info_d>());

    // Field queries
    REQUIRE(event_t.has_field("title"));
    REQUIRE(event_t.has_field("attendees"));
    REQUIRE(event_t.has_field("verbose"));
    REQUIRE(!event_t.has_field("nope"));

    REQUIRE(!event_t.field("title").has_default());
    REQUIRE(event_t.field("attendees").has_default());
    REQUIRE(event_t.field("attendees").default_value<int>() == 100);
    REQUIRE(event_t.field("verbose").has_default());
    REQUIRE(event_t.field("verbose").default_value<bool>() == false);

    // Field-level metas
    REQUIRE(!event_t.field("title").has_meta<cli_meta_d>());
    REQUIRE(!event_t.field("title").has_meta<render_meta_d>());

    REQUIRE(event_t.field("attendees").has_meta<render_meta_d>());
    REQUIRE(!event_t.field("attendees").has_meta<cli_meta_d>());
    REQUIRE(std::string_view{
        event_t.field("attendees").meta<render_meta_d>().render.style} == "bold");
    REQUIRE(event_t.field("attendees").meta<render_meta_d>().render.width == 10);

    REQUIRE(event_t.field("verbose").has_meta<cli_meta_d>());
    REQUIRE(event_t.field("verbose").has_meta<render_meta_d>());
    REQUIRE(event_t.field("verbose").meta<cli_meta_d>().cli.short_flag == 'v');
    REQUIRE(std::string_view{
        event_t.field("verbose").meta<render_meta_d>().render.style} == "dim");
}
