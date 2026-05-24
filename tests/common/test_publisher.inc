// Publisher tests — header-only-compatible (pure template + std lib).

TEST_CASE("publisher: connect + publish invokes handlers", "[publisher]") {
    collab::publisher<int> p;
    int captured = 0;
    auto sub = p.connect([&](int v) { captured = v; });
    p(42);
    REQUIRE(captured == 42);
    REQUIRE(p.subscriber_count() == 1);
}

TEST_CASE("publisher: subscription disconnect stops further calls", "[publisher]") {
    collab::publisher<int> p;
    int calls = 0;
    auto sub = p.connect([&](int) { ++calls; });
    p(1);
    sub.disconnect();
    p(2);
    REQUIRE(calls == 1);
    REQUIRE_FALSE(sub.connected());
    REQUIRE(p.subscriber_count() == 0);
}

TEST_CASE("publisher: RAII auto-disconnects on subscription destruction", "[publisher]") {
    collab::publisher<int> p;
    int calls = 0;
    {
        auto sub = p.connect([&](int) { ++calls; });
        p(1);
    }
    p(2);
    REQUIRE(calls == 1);
    REQUIRE(p.subscriber_count() == 0);
}

TEST_CASE("publisher: multi-subscriber broadcast", "[publisher]") {
    collab::publisher<int> p;
    int a = 0, b = 0, c = 0;
    auto sa = p.connect([&](int v) { a = v; });
    auto sb = p.connect([&](int v) { b = v * 2; });
    auto sc = p.connect([&](int v) { c = v * 3; });
    p(10);
    REQUIRE(a == 10);
    REQUIRE(b == 20);
    REQUIRE(c == 30);
}

TEST_CASE("publisher: subscription may safely outlive its publisher", "[publisher]") {
    collab::subscription sub;
    {
        collab::publisher<int> p;
        sub = p.connect([](int) {});
        REQUIRE(sub.connected());
    }
    REQUIRE_FALSE(sub.connected());
    sub.disconnect();  // no-op, must not crash
}

TEST_CASE("publisher: handler invokes connect/disconnect re-entrantly", "[publisher]") {
    collab::publisher<int> p;
    int outer = 0, inner = 0;
    collab::subscription inner_sub;
    auto sub = p.connect([&](int v) {
        ++outer;
        if (outer == 1) {
            inner_sub = p.connect([&](int w) { inner = w; });
        }
    });
    p(1);
    p(2);
    REQUIRE(outer == 2);
    REQUIRE(inner == 2);  // inner only fires on the second publish (added after first snapshot)
}
