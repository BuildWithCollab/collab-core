#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

import collab.core;

using namespace collab::model;

// ═════════════════════════════════════════════════════════════════════════
// Prototype: field<type_def> as a struct member
// ═════════════════════════════════════════════════════════════════════════

// The dynamic type_def's actual C++ type:
using dynamic_type_def = type_def<detail::dynamic_tag>;

// Specialization of field<> for the dynamic type_def.
// Internally stores a type_instance. Takes a type_def to construct.
// Supports with<Exts...> just like the base field<T, WithPack>.
template <typename WithPack>
struct collab::model::field<dynamic_type_def, WithPack> {
    using value_type = type_instance;

    WithPack      with{};
    type_instance value;

    // Construct from a type_def — creates a default instance
    field(const dynamic_type_def& typedef_schema)
        : value(typedef_schema.create()) {}

    // Aggregate-style construction (for with<> + type_def)
    field(WithPack with_pack, const dynamic_type_def& typedef_schema)
        : with(std::move(with_pack)), value(typedef_schema.create()) {}

    // Access the instance
    type_instance* operator->() { return &value; }
    const type_instance* operator->() const { return &value; }

    operator type_instance&() { return value; }
    operator const type_instance&() const { return value; }
};

// ── Test types ──────────────────────────────────────────────────────────

inline auto details_type = type_def("Details")
    .field<std::string>("note")
    .field<int>("priority", 5);

struct Container {
    field<std::string>       name;
    field<dynamic_type_def>  details {details_type};
};

// ── Tests ───────────────────────────────────────────────────────────────

TEST_CASE("field<type_def>: struct is constructible", "[prototype][field_type_def]") {
    Container container;
    REQUIRE(container.name.value.empty());
    REQUIRE(container.details.value.get<int>("priority") == 5);
}

TEST_CASE("field<type_def>: access nested fields via operator->", "[prototype][field_type_def]") {
    Container container;
    container.name = "parent";
    container.details->set("note", std::string("hello"));
    container.details->set("priority", 10);

    REQUIRE(container.name.value == "parent");
    REQUIRE(container.details->get<std::string>("note") == "hello");
    REQUIRE(container.details->get<int>("priority") == 10);
}

TEST_CASE("field<type_def>: defaults from nested type_def", "[prototype][field_type_def]") {
    Container container;
    REQUIRE(container.details->get<std::string>("note") == "");
    REQUIRE(container.details->get<int>("priority") == 5);
}

TEST_CASE("field<type_def>: multiple instances are independent", "[prototype][field_type_def]") {
    Container a;
    Container b;

    a.details->set("note", std::string("A"));
    b.details->set("note", std::string("B"));

    REQUIRE(a.details->get<std::string>("note") == "A");
    REQUIRE(b.details->get<std::string>("note") == "B");
}

TEST_CASE("field<type_def>: nested to_json_string via instance", "[prototype][field_type_def]") {
    Container container;
    container.details->set("note", std::string("hello"));
    container.details->set("priority", 10);

    auto json_string = container.details.value.to_json_string();
    REQUIRE(json_string.find("hello") != std::string::npos);
    REQUIRE(json_string.find("10") != std::string::npos);
}

// ── with<> on field<type_def> ────────────────────────────────────────────

struct cli_info { char short_flag = '\0'; };
struct help_info { const char* summary = ""; };

TEST_CASE("field<type_def> with with<> — single extension", "[prototype][field_type_def][with]") {
    struct WithContainer {
        field<std::string> name;
        field<dynamic_type_def, with<cli_info>> details {
            with<cli_info>{{.short_flag = 'd'}},
            details_type
        };
    };

    WithContainer wc;
    REQUIRE(wc.details.with.short_flag == 'd');
    REQUIRE(wc.details.value.get<int>("priority") == 5);
    wc.details->set("note", std::string("with works"));
    REQUIRE(wc.details->get<std::string>("note") == "with works");
}

TEST_CASE("field<type_def> with with<> — multiple extensions", "[prototype][field_type_def][with]") {
    struct MultiWithContainer {
        field<std::string> name;
        field<dynamic_type_def, with<cli_info, help_info>> details {
            with<cli_info, help_info>{{.short_flag = 'd'}, {.summary = "details field"}},
            details_type
        };
    };

    MultiWithContainer mwc;
    REQUIRE(mwc.details.with.short_flag == 'd');
    REQUIRE(std::string_view{mwc.details.with.summary} == "details field");
    REQUIRE(mwc.details.value.get<int>("priority") == 5);
}

// ── 3-level deep ────────────────────────────────────────────────────────

TEST_CASE("field<type_def>: 3-level deep nesting in structs", "[prototype][field_type_def]") {
    auto leaf_type = type_def("Leaf")
        .field<std::string>("value", std::string("deep"));

    auto middle_type = type_def("Middle")
        .field<std::string>("label", std::string("mid"))
        .field("leaf", leaf_type);

    struct MiddleHolder {
        field<std::string>       name;
        field<dynamic_type_def>  middle;

        MiddleHolder(const dynamic_type_def& middle_typedef) : middle(middle_typedef) {}
    };

    MiddleHolder holder(middle_type);
    holder.name = "top";

    REQUIRE(holder.middle->get<std::string>("label") == "mid");
    auto leaf = holder.middle->get<type_instance>("leaf");
    REQUIRE(leaf.get<std::string>("value") == "deep");
}
