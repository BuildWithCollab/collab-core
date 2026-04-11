#include <catch2/catch_test_macros.hpp>

#include <string_view>

import collab.core;

using collab::core::options;

TEST_CASE("options default construction", "[options]") {
    constexpr options o{};
    REQUIRE(std::string_view{o.desc} == "");
    REQUIRE(o.required == false);
    REQUIRE(std::string_view{o.display_name} == "");
    REQUIRE(o.hidden == false);
}

TEST_CASE("options designated initializers", "[options]") {
    constexpr options o{.desc = "x", .required = true};
    REQUIRE(std::string_view{o.desc} == "x");
    REQUIRE(o.required == true);
    REQUIRE(std::string_view{o.display_name} == "");
    REQUIRE(o.hidden == false);
}

template <options O>
struct Thing {
    static constexpr auto opts = O;
};

TEST_CASE("options usable as a non-type template parameter", "[options][nttp]") {
    using T = Thing<options{.desc = "x"}>;
    REQUIRE(std::string_view{T::opts.desc} == "x");
    STATIC_REQUIRE(T::opts.required == false);
}
