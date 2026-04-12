#include <catch2/catch_test_macros.hpp>

#include <pfr.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

import collab.core;

using namespace collab::core::fields;

// ── Domain-specific extension structs ───────────────────────────────────

struct posix_options {
    char short_flag = '\0';
    bool from_stdin = false;
    bool positional = false;
};

struct render_options {
    const char* style = "";
    int         width = 0;
};

struct posix_meta  { posix_options posix{}; };
struct render_meta { render_options render{}; };

// ── Basic instantiation and defaults ────────────────────────────────────

TEST_CASE("Field default construction", "[field]") {
    Field<int> fi;
    REQUIRE(fi.value == 0);

    Field<std::string> fs;
    REQUIRE(fs.value.empty());

    Field<bool> fb;
    REQUIRE(fb.value == false);
}

// ── Transparent conversion ──────────────────────────────────────────────

TEST_CASE("Field transparent conversion", "[field][conversion]") {
    Field<int> f;
    f = 42;
    int y = f;
    REQUIRE(y == 42);

    Field<std::string> g;
    g = "hello";
    const std::string& ref = g;
    REQUIRE(ref == "hello");
}

TEST_CASE("Field round-trips via implicit conversion", "[field][conversion]") {
    Field<int> f;
    f = 7;
    int via_conv = f;
    f = via_conv + 1;
    REQUIRE(f.value == 8);
}

// ── operator-> passthrough ──────────────────────────────────────────────

TEST_CASE("Field operator->", "[field][arrow]") {
    Field<std::string> f;
    f = "hello";
    REQUIRE(f->size() == 5);

    Field<std::string> e;
    REQUIRE(e->empty());

    Field<std::vector<int>> v;
    v.value = {1, 2, 3};
    REQUIRE(v->size() == 3);
}

// ── with<> extensions ───────────────────────────────────────────────────

TEST_CASE("Field with single extension", "[field][with]") {
    struct Args {
        Field<std::string, with<posix_meta>> query {
            .with = {{.posix = {.from_stdin = true}}}
        };
    };

    Args a{};
    REQUIRE(a.query.with.posix.from_stdin == true);
    REQUIRE(a.query.with.posix.short_flag == '\0');
}

TEST_CASE("Field with multiple extensions", "[field][with]") {
    struct Args {
        Field<bool, with<posix_meta, render_meta>> verbose {
            .with = {{.posix = {.short_flag = 'v'}}, {.render = {.style = "dimmed"}}}
        };
    };

    Args a{};
    REQUIRE(a.verbose.with.posix.short_flag == 'v');
    REQUIRE(std::string_view{a.verbose.with.render.style} == "dimmed");
}

// ── Outer struct aggregate + designated init ────────────────────────────

struct WeatherArgs {
    Field<std::string>                       city;
    Field<int>                               days    {.value = 7};
    Field<bool, with<posix_meta>>            verbose {.with = {{.posix = {.short_flag = 'v'}}}};
    Field<std::vector<std::string>, with<posix_meta, render_meta>> tags {
        .with = {{.posix = {.short_flag = 't'}}, {.render = {.width = 20}}}
    };
};

TEST_CASE("Field in outer struct", "[field][aggregate]") {
    STATIC_REQUIRE(std::is_aggregate_v<WeatherArgs>);

    WeatherArgs w{};
    REQUIRE(w.city.value.empty());
    REQUIRE(w.days.value == 7);
    REQUIRE(w.verbose.value == false);
    REQUIRE(w.verbose.with.posix.short_flag == 'v');
    REQUIRE(w.tags.with.render.width == 20);
}

// ── PFR get_name ────────────────────────────────────────────────────────

TEST_CASE("PFR get_name extracts field names", "[field][pfr]") {
    constexpr auto n0 = pfr::get_name<0, WeatherArgs>();
    constexpr auto n1 = pfr::get_name<1, WeatherArgs>();
    constexpr auto n2 = pfr::get_name<2, WeatherArgs>();
    constexpr auto n3 = pfr::get_name<3, WeatherArgs>();

    REQUIRE(n0 == "city");
    REQUIRE(n1 == "days");
    REQUIRE(n2 == "verbose");
    REQUIRE(n3 == "tags");
}

// ── LNK1179 repro: same spec, multiple structs, one TU ─────────────────

struct LoginResponse {
    Field<std::string> session_id;
    Field<std::string> user_name;
};

struct LogoutResponse {
    Field<std::string> session_id;
};

struct WhoAmI {
    Field<std::string> session_id;
    Field<std::string> display_name;
};

TEST_CASE("Same Field<std::string> across multiple structs — no LNK1179", "[field][lnk1179]") {
    LoginResponse  login{};
    LogoutResponse logout{};
    WhoAmI         whoami{};

    REQUIRE(login.session_id.value.empty());
    REQUIRE(logout.session_id.value.empty());
    REQUIRE(whoami.session_id.value.empty());
}

// Same spec WITH extensions across multiple structs:
struct SearchArgs {
    Field<std::string, with<posix_meta>> query {
        .with = {{.posix = {.from_stdin = true}}}
    };
};

struct GrepArgs {
    Field<std::string, with<posix_meta>> query {
        .with = {{.posix = {.from_stdin = true}}}
    };
};

TEST_CASE("Same Field<std::string, with<posix_meta>> across structs — no LNK1179", "[field][lnk1179]") {
    SearchArgs search{};
    GrepArgs   grep{};

    REQUIRE(search.query.with.posix.from_stdin == true);
    REQUIRE(grep.query.with.posix.from_stdin == true);
}

// ── IsField concept ─────────────────────────────────────────────────────

TEST_CASE("IsField detects Field specializations", "[field][concept]") {
    STATIC_REQUIRE(IsField<Field<int>>);
    STATIC_REQUIRE(IsField<Field<std::string>>);
    STATIC_REQUIRE(IsField<Field<std::string, with<posix_meta>>>);
    STATIC_REQUIRE(!IsField<int>);
    STATIC_REQUIRE(!IsField<std::string>);
}

// ── ReflectedStruct concept ─────────────────────────────────────────────

struct OnlyFields {
    Field<int>         a;
    Field<std::string> b;
};

struct MixedStruct {
    Field<int>           a;
    int                  plain = 0;
    Field<std::string>   b;
    std::unique_ptr<int> helper;
};

struct EmptyStruct {};

struct NoFieldsOnlyPlain {
    int x = 0;
    int y = 0;
};

struct NonAggregate {
    NonAggregate() {}
    Field<int> x;
};

TEST_CASE("ReflectedStruct accepts aggregates with Field members", "[field][concept]") {
    STATIC_REQUIRE(ReflectedStruct<OnlyFields>);
    STATIC_REQUIRE(ReflectedStruct<MixedStruct>);
    STATIC_REQUIRE(ReflectedStruct<WeatherArgs>);
    STATIC_REQUIRE(ReflectedStruct<LoginResponse>);
}

TEST_CASE("ReflectedStruct rejects non-qualifying types", "[field][concept]") {
    STATIC_REQUIRE(!ReflectedStruct<EmptyStruct>);
    STATIC_REQUIRE(!ReflectedStruct<NoFieldsOnlyPlain>);
    STATIC_REQUIRE(!ReflectedStruct<int>);
    STATIC_REQUIRE(!ReflectedStruct<NonAggregate>);
}
