#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

import collab.core;

using collab::core::Category;
using collab::core::Error;
using collab::core::make_error;

TEST_CASE("Error type shape", "[error]") {
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<Error>);
    STATIC_REQUIRE(std::is_constructible_v<Error, Category, std::string_view>);
    STATIC_REQUIRE(std::is_constructible_v<Error, Category, const char*>);
    STATIC_REQUIRE(std::is_constructible_v<Error, Category, std::string>);
    STATIC_REQUIRE(std::is_copy_constructible_v<Error>);
    STATIC_REQUIRE(std::is_move_constructible_v<Error>);
}

TEST_CASE("Error stores category and message", "[error]") {
    Error e{Category::not_found, "config.yaml missing"};
    REQUIRE(e.category == Category::not_found);
    REQUIRE(e.message == "config.yaml missing");
}

TEST_CASE("Error message owns its storage", "[error]") {
    std::string source = "transient buffer";
    Error       e{Category::network, source};
    source.clear();
    REQUIRE(e.message == "transient buffer");
}

TEST_CASE("make_error round-trips category and produces formatted message", "[error]") {
    Error e = make_error(Category::not_found, "config '{}' missing", "app.yaml");
    REQUIRE(e.category == Category::not_found);
    REQUIRE(e.message == "config 'app.yaml' missing");
}

TEST_CASE("make_error formats multiple args of mixed types", "[error]") {
    Error e = make_error(
        Category::network,
        "request to {} failed after {} retries (status {})",
        "api.example.com",
        3,
        503);
    REQUIRE(e.message == "request to api.example.com failed after 3 retries (status 503)");
}

TEST_CASE("make_error supports type-specific format specs", "[error]") {
    Error e = make_error(
        Category::timeout, "elapsed {:.2f}s waiting on handle {:#x}", 1.23456, 0xdead);
    REQUIRE(e.message == "elapsed 1.23s waiting on handle 0xdead");
}

TEST_CASE("make_error supports width and alignment specs", "[error]") {
    Error e = make_error(Category::usage, "field [{:>8}] empty", "name");
    REQUIRE(e.message == "field [    name] empty");
}

TEST_CASE("make_error supports indexed args", "[error]") {
    Error e = make_error(
        Category::conflict, "{0} conflicts with itself ({0} != {0}?)", "version");
    REQUIRE(e.message == "version conflicts with itself (version != version?)");
}

TEST_CASE("make_error works with no format args", "[error]") {
    Error e = make_error(Category::cancelled, "user aborted");
    REQUIRE(e.category == Category::cancelled);
    REQUIRE(e.message == "user aborted");
}

TEST_CASE("make_error forwards rvalue args", "[error]") {
    std::string owned = "abc";
    Error       e     = make_error(Category::not_found, "missing: {}", std::move(owned));
    REQUIRE(e.message == "missing: abc");
}
