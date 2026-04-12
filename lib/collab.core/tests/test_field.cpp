// NOTE: Field<> specializations in this file use deliberately unique Name
// NTTPs (e.g. "s_arrow", "s_empty", "s_default") rather than sharing a
// single "s" across multiple TEST_CASEs. This is a workaround for an
// MSVC 14.50 linker bug (LNK1179) where the same Field<Name, T, ...>
// specialization referenced more than once textually in one translation
// unit, with a non-trivially-destructible T, causes duplicate COMDAT
// emission of Field's implicit dtor/ctor. See the block comment at the
// top of lib/collab.core/src/field.cppm for full details.
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

import collab.core;

using collab::core::Field;
using collab::core::IsField;
using collab::core::ReflectedStruct;
using collab::core::options;

// ── Test extensions (all structural types for NTTP use) ─────────────────

struct test_ext {
    int         number = 0;
    const char* tag    = "";
    bool        flag   = false;
};
static_assert(std::is_standard_layout_v<test_ext>);

struct ext_a {
    int a = 0;
};

struct ext_b {
    float b = 0.0f;
};

// ── Basic instantiation and defaults ────────────────────────────────────

TEST_CASE("Field default construction", "[field]") {
    Field<"test", int> f;
    REQUIRE(f.value == 0);
    REQUIRE(std::string_view{decltype(f)::field_core().desc} == "");
    REQUIRE(decltype(f)::field_core().required == false);
    STATIC_REQUIRE(std::is_same_v<decltype(f)::type, int>);
    REQUIRE(std::string_view{decltype(f)::field_name().c_str()} == "test");

    Field<"s_default", std::string> fs;
    REQUIRE(fs.value.empty());

    Field<"b", bool> fb;
    REQUIRE(fb.value == false);
}

// ── Core options via NTTP ───────────────────────────────────────────────

TEST_CASE("Field carries core options via NTTP", "[field][options]") {
    using CityField = Field<"city_opts", std::string, options{.desc = "City name", .required = true}>;
    STATIC_REQUIRE(std::string_view{CityField::field_core().desc} == "City name");
    STATIC_REQUIRE(CityField::field_core().required == true);

    using DaysField =
        Field<"days_opts", int, options{.desc = "Days", .display_name = "Days to Forecast"}>;
    STATIC_REQUIRE(std::string_view{DaysField::field_core().display_name} == "Days to Forecast");

    using HiddenField = Field<"hidden_field", int, options{.hidden = true}>;
    STATIC_REQUIRE(HiddenField::field_core().hidden == true);
}

// ── Transparent conversion ──────────────────────────────────────────────

TEST_CASE("Field is transparently convertible to its wrapped type", "[field][conversion]") {
    Field<"x_conv", int> f;
    f = 42;
    int y = f;
    REQUIRE(y == 42);

    Field<"s_conv", std::string> g;
    g                      = "hello";
    const std::string& ref = g;
    REQUIRE(ref == "hello");
}

TEST_CASE("Field round-trips via implicit conversion", "[field][conversion]") {
    Field<"n_round", int> f;
    f = 7;
    int via_conv = f;
    f            = via_conv + 1;
    REQUIRE(f.value == 8);
}

// ── operator-> passthrough ──────────────────────────────────────────────

TEST_CASE("Field operator-> delegates to the wrapped value", "[field][arrow]") {
    Field<"s_arrow", std::string> f;
    f = "hello";
    REQUIRE(f->size() == 5);

    Field<"s_empty", std::string> e;
    REQUIRE(e->empty());

    Field<"v_arrow", std::vector<int>> v;
    v.value = {1, 2, 3};
    REQUIRE(v->size() == 3);
}

// ── Use as a member of an outer aggregate ───────────────────────────────

struct TestArgs {
    Field<"city", std::string, options{.desc = "City", .required = true}> city;
    Field<"days", int, options{.desc = "Days"}>                           days{7};
};

TEST_CASE("Field preserves outer struct aggregate-initializability", "[field][aggregate]") {
    STATIC_REQUIRE(std::is_aggregate_v<Field<"agg_int", int>>);
    STATIC_REQUIRE(std::is_aggregate_v<TestArgs>);

    TestArgs a{};
    REQUIRE(a.city.value.empty());
    REQUIRE(a.days.value == 7);

    TestArgs b{.days = {14}};
    REQUIRE(b.days.value == 14);
    REQUIRE(b.city.value.empty());

    TestArgs c{};
    c.city = "Portland";
    REQUIRE(std::string(c.city) == "Portland");
}

// ── Extension lookup (single) ───────────────────────────────────────────

TEST_CASE("Field::ext<E> returns the matching extension", "[field][ext]") {
    using F = Field<"x_ext_single", int, {}, test_ext{.number = 42, .tag = "foo"}>;
    STATIC_REQUIRE(F::template ext<test_ext>().number == 42);
    STATIC_REQUIRE(std::string_view{F::template ext<test_ext>().tag} == "foo");
}

TEST_CASE("Field::ext<E> returns a default-constructed E when absent", "[field][ext]") {
    using F = Field<"x_ext_absent", int>;
    STATIC_REQUIRE(F::template ext<test_ext>().number == 0);
    STATIC_REQUIRE(F::template ext<test_ext>().flag == false);
}

TEST_CASE("Field::has_ext reports extension presence", "[field][ext]") {
    STATIC_REQUIRE(Field<"x_hasext_yes", int, {}, test_ext{}>::template has_ext<test_ext>() == true);
    STATIC_REQUIRE(Field<"x_hasext_no", int>::template has_ext<test_ext>() == false);
}

// ── Multi-extension pack ────────────────────────────────────────────────

TEST_CASE("Field supports multiple distinct extensions", "[field][ext][multi]") {
    using F = Field<"x_multi", int, {}, ext_a{.a = 11}, ext_b{.b = 2.5f}>;
    STATIC_REQUIRE(F::template ext<ext_a>().a == 11);
    STATIC_REQUIRE(F::template ext<ext_b>().b == 2.5f);
    STATIC_REQUIRE(F::template has_ext<ext_a>());
    STATIC_REQUIRE(F::template has_ext<ext_b>());
}

// ── Zero-extension pack ─────────────────────────────────────────────────

TEST_CASE("Field with an empty extension pack", "[field][ext][empty]") {
    STATIC_REQUIRE(Field<"x_empty_pack_a", int>::template has_ext<test_ext>() == false);
    STATIC_REQUIRE(
        Field<"x_empty_pack_b", int, options{.desc = "only core"}>::template has_ext<test_ext>()
        == false
    );
}

// ── IsField concept ─────────────────────────────────────────────────────

TEST_CASE("IsField detects Field specializations", "[field][concept][is_field]") {
    STATIC_REQUIRE(IsField<Field<"x_isfield", int>>);
    STATIC_REQUIRE(IsField<Field<"y_isfield", std::string, options{.desc = "y"}>>);
    STATIC_REQUIRE(!IsField<int>);
    STATIC_REQUIRE(!IsField<std::optional<int>>);
    STATIC_REQUIRE(!IsField<std::string>);
}

// ── ReflectedStruct concept ─────────────────────────────────────────────

struct OnlyFields {
    Field<"a_only", int>         a;
    Field<"b_only", std::string> b;
};

struct MixedStruct {
    Field<"a_mixed", int>         a;
    int                           plain = 0;   // non-Field, invisible to the framework
    Field<"b_mixed", std::string> b;
    std::unique_ptr<int>          helper;      // non-Field, invisible; doesn't break aggregate
};

struct EmptyStruct {};  // aggregate but zero members

struct NoFieldsOnlyPlain {
    int x = 0;
    int y = 0;
};

struct NonAggregate {
    NonAggregate() {}  // user-declared constructor defeats aggregate status
    Field<"x_nonagg", int> x;
};

TEST_CASE("ReflectedStruct accepts aggregates with at least one Field member", "[field][concept][reflected]") {
    STATIC_REQUIRE(ReflectedStruct<OnlyFields>);
    STATIC_REQUIRE(ReflectedStruct<MixedStruct>);
    STATIC_REQUIRE(ReflectedStruct<TestArgs>);
}

TEST_CASE("ReflectedStruct rejects non-qualifying types", "[field][concept][reflected]") {
    STATIC_REQUIRE(!ReflectedStruct<EmptyStruct>);
    STATIC_REQUIRE(!ReflectedStruct<NoFieldsOnlyPlain>);
    STATIC_REQUIRE(!ReflectedStruct<int>);
    STATIC_REQUIRE(!ReflectedStruct<NonAggregate>);
}
