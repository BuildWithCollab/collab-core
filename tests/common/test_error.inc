// Error tests — header-only-compatible (fmt is linked into the test).

TEST_CASE("error: construction from C-string", "[error]") {
    try {
        throw collab::error("boom");
    } catch (const std::runtime_error& e) {
        REQUIRE(std::string(e.what()) == "boom");
    }
}

TEST_CASE("error: construction from std::string", "[error]") {
    try {
        throw collab::error(std::string("named"));
    } catch (const std::runtime_error& e) {
        REQUIRE(std::string(e.what()) == "named");
    }
}

TEST_CASE("error: construction from string_view", "[error]") {
    std::string_view sv = "view";
    try {
        throw collab::error(sv);
    } catch (const std::runtime_error& e) {
        REQUIRE(std::string(e.what()) == "view");
    }
}

TEST_CASE("error: fmt-style formatted construction", "[error]") {
    try {
        throw collab::error("bad value {}: expected {}", 7, "positive");
    } catch (const std::runtime_error& e) {
        REQUIRE(std::string(e.what()) == "bad value 7: expected positive");
    }
}
