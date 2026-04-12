// [EXPERIMENT Q+R] Shapes A and F, using ext_pack<Exts...> member instead
// of direct inheritance, so all Field-level init is designated.
#include <catch2/catch_test_macros.hpp>

#include <boost/pfr.hpp>

#include <string>
#include <string_view>
#include <vector>

import collab.core;

using collab::core::options;

// ── Fake subsystem metadata types ───────────────────────────────────

struct posix_options {
    char short_flag = '\0';
    bool from_stdin = false;
    bool positional = false;
};

struct render_options {
    const char* style = "";
    int         width = 0;
};

struct posix_ext  { posix_options posix{}; };
struct render_ext { render_options render{}; };

// ── ext_pack: inherits from all Exts, becomes a single member ───────

template <typename... Exts>
struct ext_pack : Exts... {};

// ═════════════════════════════════════════════════════════════════════
// SHAPE A — nested .core member
// ═════════════════════════════════════════════════════════════════════

template <typename T, typename ExtPack = ext_pack<>>
struct FieldA {
    ExtPack exts{};
    T       value{};
    options core{};

    constexpr operator const T&() const& noexcept { return value; }
    constexpr operator       T&()       & noexcept { return value; }
};

struct WeatherA {
    FieldA<std::string>                        city    {.core = {.desc = "City", .required = true}};
    FieldA<std::string, ext_pack<posix_ext>>   query   {.exts = {{.posix = {.from_stdin = true}}}, .core = {.desc = "Query"}};
    FieldA<bool, ext_pack<posix_ext>>          verbose {.exts = {{.posix = {.short_flag = 'v'}}}, .core = {.desc = "Detail"}};
    FieldA<std::vector<std::string>, ext_pack<posix_ext, render_ext>> tags {
        .exts = {{.posix = {.short_flag = 't'}}, {.render = {.width = 20}}},
        .core = {.desc = "Tags"}
    };
};

struct SearchA {
    FieldA<std::string, ext_pack<posix_ext>> query {.exts = {{.posix = {.from_stdin = true}}}, .core = {.desc = "Search query"}};
};

TEST_CASE("shape A: basic usage", "[shape-a]") {
    WeatherA w{};
    REQUIRE(w.city.value.empty());
    REQUIRE(std::string_view{w.city.core.desc} == "City");
    REQUIRE(w.city.core.required == true);
    REQUIRE(w.query.exts.posix.from_stdin == true);
    REQUIRE(w.verbose.exts.posix.short_flag == 'v');
    REQUIRE(w.tags.exts.render.width == 20);
    REQUIRE(w.tags.exts.posix.short_flag == 't');
}

TEST_CASE("shape A: LNK1179 repro", "[shape-a]") {
    WeatherA w{};
    SearchA  s{};
    REQUIRE(std::string_view{w.query.core.desc} == "Query");
    REQUIRE(std::string_view{s.query.core.desc} == "Search query");
}

// ═════════════════════════════════════════════════════════════════════
// SHAPE F — flat core members
// ═════════════════════════════════════════════════════════════════════

template <typename T, typename ExtPack = ext_pack<>>
struct FieldF {
    ExtPack     exts{};
    T           value{};
    const char* desc         = "";
    bool        required     = false;
    const char* display_name = "";
    bool        hidden       = false;

    constexpr operator const T&() const& noexcept { return value; }
    constexpr operator       T&()       & noexcept { return value; }
};

struct WeatherF {
    FieldF<std::string>                        city    {.desc = "City", .required = true};
    FieldF<std::string, ext_pack<posix_ext>>   query   {.exts = {{.posix = {.from_stdin = true}}}, .desc = "Query"};
    FieldF<bool, ext_pack<posix_ext>>          verbose {.exts = {{.posix = {.short_flag = 'v'}}}, .desc = "Detail"};
    FieldF<std::vector<std::string>, ext_pack<posix_ext, render_ext>> tags {
        .exts = {{.posix = {.short_flag = 't'}}, {.render = {.width = 20}}},
        .desc = "Tags"
    };
};

struct SearchF {
    FieldF<std::string, ext_pack<posix_ext>> query {.exts = {{.posix = {.from_stdin = true}}}, .desc = "Search query"};
};

TEST_CASE("shape F: basic usage", "[shape-f]") {
    WeatherF w{};
    REQUIRE(w.city.value.empty());
    REQUIRE(std::string_view{w.city.desc} == "City");
    REQUIRE(w.city.required == true);
    REQUIRE(w.query.exts.posix.from_stdin == true);
    REQUIRE(w.verbose.exts.posix.short_flag == 'v');
    REQUIRE(w.tags.exts.render.width == 20);
    REQUIRE(w.tags.exts.posix.short_flag == 't');
}

TEST_CASE("shape F: LNK1179 repro", "[shape-f]") {
    WeatherF w{};
    SearchF  s{};
    REQUIRE(std::string_view{w.query.desc} == "Query");
    REQUIRE(std::string_view{s.query.desc} == "Search query");
}

TEST_CASE("shape F: PFR get_name", "[shape-f][pfr]") {
    constexpr auto n0 = boost::pfr::get_name<0, WeatherF>();
    constexpr auto n1 = boost::pfr::get_name<1, WeatherF>();
    REQUIRE(n0 == "city");
    REQUIRE(n1 == "query");
}
