#include "test_model_types.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// has_meta() — detects present metas
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: has_meta() detects present metas", "[type_def][typed][has_meta]") {
    type_def<Dog> t;
    REQUIRE(t.has_meta<endpoint_info>());
    REQUIRE(t.has_meta<help_info>());
}

TEST_CASE("dynamic: has_meta()", "[type_def][dynamic][has_meta]") {
    auto t = type_def("Event")
        .meta<endpoint_info>({.path = "/events"});
    REQUIRE(t.has_meta<endpoint_info>());
    REQUIRE(!t.has_meta<help_info>());
}

// ═══════════════════════════════════════════════════════════════════════════
// has_meta() — returns false for absent metas
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: has_meta() returns false for absent metas", "[type_def][typed][has_meta]") {
    type_def<Dog> t;
    REQUIRE(!t.has_meta<tag_info>());
    REQUIRE(!t.has_meta<cli_meta>());
}

TEST_CASE("dynamic: has_meta() false when no metas", "[type_def][dynamic][has_meta]") {
    auto t = type_def("Simple")
        .field<int>("x");
    REQUIRE(!t.has_meta<endpoint_info>());
}

// ═══════════════════════════════════════════════════════════════════════════
// has_meta() — struct with no metas
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: has_meta() returns false when struct has no metas", "[type_def][typed][has_meta]") {
    type_def<SimpleArgs> t;
    REQUIRE(!t.has_meta<endpoint_info>());
    REQUIRE(!t.has_meta<help_info>());
}

// ═══════════════════════════════════════════════════════════════════════════
// has_meta() — multi-tagged struct
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: has_meta() works for multi-tagged struct", "[type_def][typed][has_meta]") {
    type_def<MultiTagged> t;
    REQUIRE(t.has_meta<tag_info>());
    REQUIRE(!t.has_meta<endpoint_info>());
}

// ═══════════════════════════════════════════════════════════════════════════
// meta() — returns the metadata value
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: meta() returns the metadata value", "[type_def][typed][meta]") {
    type_def<Dog> t;
    auto ep = t.meta<endpoint_info>();
    REQUIRE(std::string_view{ep.path} == "/dogs");
    REQUIRE(std::string_view{ep.method} == "POST");
}

TEST_CASE("dynamic: meta()", "[type_def][dynamic][meta]") {
    auto t = type_def("Event")
        .meta<endpoint_info>({.path = "/events", .method = "POST"});
    auto ep = t.meta<endpoint_info>();
    REQUIRE(std::string_view{ep.path} == "/events");
    REQUIRE(std::string_view{ep.method} == "POST");
}

// ═══════════════════════════════════════════════════════════════════════════
// meta() — returns different metadata types
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: meta() returns different metadata types", "[type_def][typed][meta]") {
    type_def<Dog> t;
    auto h = t.meta<help_info>();
    REQUIRE(std::string_view{h.summary} == "A good boy");
}

// ═══════════════════════════════════════════════════════════════════════════
// meta() — returns first when multiple of same type
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: meta() returns first when multiple of same type", "[type_def][typed][meta]") {
    type_def<MultiTagged> t;
    auto tag = t.meta<tag_info>();
    REQUIRE(std::string_view{tag.value} == "pet");
}

TEST_CASE("dynamic: meta() returns first when multiple", "[type_def][dynamic][meta]") {
    auto t = type_def("Tagged")
        .meta<tag_info>({.value = "first"})
        .meta<tag_info>({.value = "second"});
    REQUIRE(std::string_view{t.meta<tag_info>().value} == "first");
}

// ═══════════════════════════════════════════════════════════════════════════
// meta_count()
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: meta_count() returns correct count", "[type_def][typed][meta_count]") {
    REQUIRE(type_def<Dog>{}.meta_count<endpoint_info>() == 1);
    REQUIRE(type_def<Dog>{}.meta_count<help_info>() == 1);
    REQUIRE(type_def<Dog>{}.meta_count<tag_info>() == 0);
}

TEST_CASE("typed: meta_count() for multiple metas of same type", "[type_def][typed][meta_count]") {
    REQUIRE(type_def<MultiTagged>{}.meta_count<tag_info>() == 3);
}

TEST_CASE("typed: meta_count() is zero for no-meta struct", "[type_def][typed][meta_count]") {
    REQUIRE(type_def<SimpleArgs>{}.meta_count<endpoint_info>() == 0);
}

TEST_CASE("dynamic: meta_count()", "[type_def][dynamic][meta_count]") {
    auto t = type_def("Tagged")
        .meta<tag_info>({.value = "a"})
        .meta<tag_info>({.value = "b"})
        .meta<tag_info>({.value = "c"});
    REQUIRE(t.meta_count<tag_info>() == 3);
    REQUIRE(t.meta_count<endpoint_info>() == 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// metas()
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: metas() returns all metas of a given type", "[type_def][typed][metas]") {
    type_def<MultiTagged> t;
    auto tags = t.metas<tag_info>();
    REQUIRE(tags.size() == 3);
    REQUIRE(std::string_view{tags[0].value} == "pet");
    REQUIRE(std::string_view{tags[1].value} == "animal");
    REQUIRE(std::string_view{tags[2].value} == "good-boy");
}

TEST_CASE("typed: metas() returns single-element vector for one meta", "[type_def][typed][metas]") {
    type_def<Dog> t;
    auto eps = t.metas<endpoint_info>();
    REQUIRE(eps.size() == 1);
    REQUIRE(std::string_view{eps[0].path} == "/dogs");
}

TEST_CASE("dynamic: metas()", "[type_def][dynamic][metas]") {
    auto t = type_def("Tagged")
        .meta<tag_info>({.value = "a"})
        .meta<tag_info>({.value = "b"});
    auto tags = t.metas<tag_info>();
    REQUIRE(tags.size() == 2);
    REQUIRE(std::string_view{tags[0].value} == "a");
    REQUIRE(std::string_view{tags[1].value} == "b");
}

// ═══════════════════════════════════════════════════════════════════════════
// Meta-only struct
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: meta-only struct has zero fields but metas work", "[type_def][typed][meta]") {
    type_def<MetaOnly> t;
    REQUIRE(t.field_count() == 0);
    REQUIRE(t.has_meta<endpoint_info>());
    auto ep = t.meta<endpoint_info>();
    REQUIRE(std::string_view{ep.path} == "/health");
}
