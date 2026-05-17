#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

import collab.core;

using collab::core::emitter;
using collab::core::subscription;

// ─────────────────────────────────────────────────────────────────────────────
// 1–4: Basic connect / emit / disconnect
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("emitter<int>: single subscriber receives an emit", "[emitter][basic]") {
    emitter<int> e;
    int          received = 0;
    auto         sub      = e.connect([&](int x) { received = x; });

    e(42);

    REQUIRE(received == 42);
}

TEST_CASE("emitter<>: multiple subscribers all fire in connection order",
          "[emitter][basic]") {
    emitter<>        e;
    std::vector<int> order;

    auto s1 = e.connect([&] { order.push_back(1); });
    auto s2 = e.connect([&] { order.push_back(2); });
    auto s3 = e.connect([&] { order.push_back(3); });

    e();

    REQUIRE(order == std::vector<int>{1, 2, 3});
}

TEST_CASE("emitter<>: emit with zero subscribers is a no-op", "[emitter][basic]") {
    emitter<int> e;
    REQUIRE_NOTHROW(e(7));
    REQUIRE(e.subscriber_count() == 0);
}

TEST_CASE("subscriber_count() reflects connect and disconnect",
          "[emitter][basic]") {
    emitter<> e;
    REQUIRE(e.subscriber_count() == 0);

    auto s1 = e.connect([] {});
    auto s2 = e.connect([] {});
    REQUIRE(e.subscriber_count() == 2);

    s1.disconnect();
    REQUIRE(e.subscriber_count() == 1);

    {
        auto s3 = e.connect([] {});
        REQUIRE(e.subscriber_count() == 2);
    }  // s3's destructor disconnects
    REQUIRE(e.subscriber_count() == 1);
}

// ─────────────────────────────────────────────────────────────────────────────
// 5–8: subscription lifetime
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("subscription destructor disconnects", "[emitter][lifetime]") {
    emitter<> e;
    int       calls = 0;
    {
        auto sub = e.connect([&] { ++calls; });
        e();
        REQUIRE(calls == 1);
    }
    e();
    REQUIRE(calls == 1);
    REQUIRE(e.subscriber_count() == 0);
}

TEST_CASE("Explicit disconnect() removes the handler", "[emitter][lifetime]") {
    emitter<> e;
    int       calls = 0;
    auto      sub   = e.connect([&] { ++calls; });

    e();
    REQUIRE(calls == 1);

    sub.disconnect();
    e();
    REQUIRE(calls == 1);
    REQUIRE_FALSE(sub.connected());
}

TEST_CASE("disconnect() is idempotent", "[emitter][lifetime]") {
    emitter<> e;
    auto      sub = e.connect([] {});

    REQUIRE_NOTHROW(sub.disconnect());
    REQUIRE_NOTHROW(sub.disconnect());
    REQUIRE_NOTHROW(sub.disconnect());
    REQUIRE(e.subscriber_count() == 0);
}

TEST_CASE("subscription safely outlives its emitter", "[emitter][lifetime]") {
    subscription sub;
    {
        emitter<int> e;
        sub = e.connect([](int) {});
        REQUIRE(sub.connected());
    }
    // emitter is destroyed; subscription is now orphaned.
    REQUIRE_FALSE(sub.connected());
    REQUIRE_NOTHROW(sub.disconnect());
}  // sub destroyed here — its destructor must not crash

// ─────────────────────────────────────────────────────────────────────────────
// 9–11: Reentrancy from inside a handler
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Handler self-disconnects mid-emit", "[emitter][reentrancy]") {
    emitter<> e;
    int       calls_a = 0;
    int       calls_b = 0;
    int       calls_c = 0;

    subscription sub_b;

    auto sub_a = e.connect([&] { ++calls_a; });
    sub_b      = e.connect([&] {
        ++calls_b;
        sub_b.disconnect();
    });
    auto sub_c = e.connect([&] { ++calls_c; });

    e();
    // First emit: snapshot included all three. b disconnects itself, but
    // c was already in the snapshot and still fires.
    REQUIRE(calls_a == 1);
    REQUIRE(calls_b == 1);
    REQUIRE(calls_c == 1);
    REQUIRE(e.subscriber_count() == 2);

    e();
    // Second emit: b is gone.
    REQUIRE(calls_a == 2);
    REQUIRE(calls_b == 1);
    REQUIRE(calls_c == 2);
}

TEST_CASE("Handler connects new handler mid-emit", "[emitter][reentrancy]") {
    emitter<>    e;
    int          outer_calls = 0;
    int          inner_calls = 0;
    subscription inner_sub;

    auto outer_sub = e.connect([&] {
        ++outer_calls;
        if (!inner_sub.connected())
            inner_sub = e.connect([&] { ++inner_calls; });
    });

    e();
    // Newly-connected inner is NOT in the in-flight snapshot.
    REQUIRE(outer_calls == 1);
    REQUIRE(inner_calls == 0);

    e();
    // Now inner is in the snapshot.
    REQUIRE(outer_calls == 2);
    REQUIRE(inner_calls == 1);
}

TEST_CASE("Recursive emit on the same emitter does not deadlock",
          "[emitter][reentrancy]") {
    emitter<int> e;
    int          total_calls = 0;

    auto sub = e.connect([&](int depth) {
        ++total_calls;
        if (depth > 0) e(depth - 1);
    });

    e(3);
    // depth=3 → 2 → 1 → 0  (4 invocations)
    REQUIRE(total_calls == 4);
}

// ─────────────────────────────────────────────────────────────────────────────
// 12–16: Type-erased payloads
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("emitter<>: void signature", "[emitter][payload]") {
    emitter<> e;
    int       calls = 0;
    auto      sub   = e.connect([&] { ++calls; });
    e();
    e();
    REQUIRE(calls == 2);
}

TEST_CASE("emitter<int>: primitive arg", "[emitter][payload]") {
    emitter<int> e;
    int          received = 0;
    auto         sub      = e.connect([&](int x) { received = x; });
    e(123);
    REQUIRE(received == 123);
}

TEST_CASE("emitter<const std::string&>: by-const-ref arg", "[emitter][payload]") {
    emitter<const std::string&> e;
    std::string                 received;
    const void*                 seen_address = nullptr;

    auto sub = e.connect([&](const std::string& s) {
        received     = s;
        seen_address = &s;
    });

    std::string source = "hello";
    e(source);

    REQUIRE(received == "hello");
    REQUIRE(seen_address != nullptr);
}

TEST_CASE("emitter<int, double, const std::string&>: multi-arg",
          "[emitter][payload]") {
    emitter<int, double, const std::string&> e;
    int                                      i = 0;
    double                                   d = 0.0;
    std::string                              s;

    auto sub = e.connect(
        [&](int a, double b, const std::string& c) {
            i = a;
            d = b;
            s = c;
        });

    e(7, 3.14, std::string{"pi"});

    REQUIRE(i == 7);
    REQUIRE(d == 3.14);
    REQUIRE(s == "pi");
}

namespace {
struct payload {
    int         n;
    std::string label;
};
}  // namespace

TEST_CASE("emitter<MyStruct>: user-defined type by value", "[emitter][payload]") {
    emitter<payload> e;
    payload          received{};

    auto sub = e.connect([&](payload p) { received = std::move(p); });
    e(payload{.n = 9, .label = "nine"});

    REQUIRE(received.n == 9);
    REQUIRE(received.label == "nine");
}

// ─────────────────────────────────────────────────────────────────────────────
// 17–18: Concurrency
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Concurrent connect / emit / subscriber_count under thrash",
          "[emitter][concurrency]") {
    emitter<int>      e;
    std::atomic<bool> stop{false};

    // Keep one always-on subscriber so emit always has work to do.
    std::atomic<int> baseline_calls{0};
    auto baseline_sub = e.connect([&](int) { baseline_calls.fetch_add(1); });

    // Connector: continuously connects + drops subscriptions.
    std::thread connector([&] {
        while (!stop.load(std::memory_order_relaxed)) {
            auto s = e.connect([](int) {});
            // s drops at end of scope → auto-disconnect
        }
    });

    // Emitter: continuously fires.
    std::thread emitter_thread([&] {
        int i = 0;
        while (!stop.load(std::memory_order_relaxed)) e(i++);
    });

    // Counter: continuously queries subscriber_count.
    std::thread counter([&] {
        std::size_t total = 0;
        while (!stop.load(std::memory_order_relaxed))
            total += e.subscriber_count();
        REQUIRE(total >= 0);  // exists only to prevent the loop being elided
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    stop.store(true, std::memory_order_relaxed);

    connector.join();
    emitter_thread.join();
    counter.join();

    REQUIRE(baseline_calls.load() > 0);
}

TEST_CASE("Concurrent emit() from multiple threads", "[emitter][concurrency]") {
    emitter<int>     e;
    std::atomic<int> calls{0};

    auto sub = e.connect([&](int) { calls.fetch_add(1); });

    constexpr int    iterations = 5'000;
    std::thread      a([&] {
        for (int i = 0; i < iterations; ++i) e(i);
    });
    std::thread      b([&] {
        for (int i = 0; i < iterations; ++i) e(i);
    });

    a.join();
    b.join();

    REQUIRE(calls.load() == iterations * 2);
}

// ─────────────────────────────────────────────────────────────────────────────
// 19: Compile-time behavior
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("connect() is [[nodiscard]] (signature check)",
          "[emitter][compile_time]") {
    // The [[nodiscard]] attribute itself can only be observed via compiler
    // diagnostics — there is no portable trait for it. This test pins the
    // signature so any silent change to the return type is caught, and
    // documents the contract: discarding connect()'s return value MUST
    // produce a compiler diagnostic.
    emitter<int> e;
    using ReturnT = decltype(e.connect([](int) {}));
    STATIC_REQUIRE(std::is_same_v<ReturnT, subscription>);

    // Sanity: subscription is move-only.
    STATIC_REQUIRE(std::is_move_constructible_v<subscription>);
    STATIC_REQUIRE(std::is_move_assignable_v<subscription>);
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<subscription>);
    STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<subscription>);

    // emitter is pinned (neither copyable nor movable).
    STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<emitter<int>>);
    STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<emitter<int>>);
    STATIC_REQUIRE_FALSE(std::is_move_constructible_v<emitter<int>>);
    STATIC_REQUIRE_FALSE(std::is_move_assignable_v<emitter<int>>);
}
