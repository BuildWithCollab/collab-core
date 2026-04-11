#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

import collab.core;

using collab::core::Error;
namespace ec = collab::core::error_category;

TEST_CASE("Error default construction", "[error]") {
    Error e;
    REQUIRE(e.message == "");
    REQUIRE(e.category == "general");
}

TEST_CASE("Error explicit construction", "[error]") {
    Error e{.message = "boom", .category = std::string{ec::network}};
    REQUIRE(e.message == "boom");
    REQUIRE(e.category == "network");
}

TEST_CASE("error_category constants match their names", "[error]") {
    STATIC_REQUIRE(ec::general == std::string_view{"general"});
    STATIC_REQUIRE(ec::usage == std::string_view{"usage"});
    STATIC_REQUIRE(ec::not_found == std::string_view{"not_found"});
    STATIC_REQUIRE(ec::permission == std::string_view{"permission"});
    STATIC_REQUIRE(ec::network == std::string_view{"network"});
    STATIC_REQUIRE(ec::timeout == std::string_view{"timeout"});
    STATIC_REQUIRE(ec::conflict == std::string_view{"conflict"});
    STATIC_REQUIRE(ec::unavailable == std::string_view{"unavailable"});
    STATIC_REQUIRE(ec::internal == std::string_view{"internal"});
}
