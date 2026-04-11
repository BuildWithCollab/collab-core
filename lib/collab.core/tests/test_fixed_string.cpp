#include <catch2/catch_test_macros.hpp>

#include <string_view>

import collab.core;

using collab::core::fixed_string;

TEST_CASE("fixed_string constructs from a string literal", "[fixed_string]") {
    constexpr fixed_string s{"hello"};
    STATIC_REQUIRE(s.size() == 5);
    STATIC_REQUIRE(s.c_str()[0] == 'h');
    REQUIRE(std::string_view{s.c_str(), s.size()} == "hello");
}

TEST_CASE("fixed_string equality", "[fixed_string]") {
    constexpr fixed_string a{"foo"};
    constexpr fixed_string b{"foo"};
    constexpr fixed_string c{"bar"};
    STATIC_REQUIRE(a == b);
    STATIC_REQUIRE(!(a == c));
}

template <fixed_string Name>
struct Tagged {
    static constexpr auto tag = Name;
};

TEST_CASE("fixed_string usable as a non-type template parameter", "[fixed_string][nttp]") {
    using T = Tagged<"test">;
    STATIC_REQUIRE(T::tag.size() == 4);
    REQUIRE(std::string_view{T::tag.c_str()} == "test");
}
