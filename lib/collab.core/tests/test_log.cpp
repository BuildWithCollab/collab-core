#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

import collab.core;

using namespace collab::log;

// ── Test sink that captures messages ────────────────────────────────

class capture_sink final : public sink {
public:
    struct entry {
        level       lvl;
        std::string msg;
    };

    void write(level lvl, std::string_view msg) override {
        entries.push_back({lvl, std::string(msg)});
    }

    std::vector<entry> entries;
};

// ── Helpers ─────────────────────────────────────────────────────────

struct log_fixture {
    log_fixture() {
        clear_sinks();
        set_level(level::trace);
    }
    ~log_fixture() {
        clear_sinks();
        set_level(level::info);
    }
};

static auto make_capture() {
    auto owned = std::make_unique<capture_sink>();
    auto* raw = owned.get();
    add_sink(std::move(owned));
    return raw;
}

// ── Tests ───────────────────────────────────────────────────────────

TEST_CASE("logging with no sinks does not crash", "[log]") {
    log_fixture fix;
    info("hello");
    warn("warning");
    error("error");
    critical("critical");
}

TEST_CASE("log messages are dispatched to sinks", "[log]") {
    log_fixture fix;
    auto* cap = make_capture();

    info("hello world");
    REQUIRE(cap->entries.size() == 1);
    CHECK(cap->entries[0].lvl == level::info);
    CHECK(cap->entries[0].msg == "hello world");
}

TEST_CASE("level filtering works", "[log]") {
    log_fixture fix;
    auto* cap = make_capture();

    set_level(level::warn);

    trace("t");
    debug("d");
    info("i");
    warn("w");
    error("e");
    critical("c");

    REQUIRE(cap->entries.size() == 3);
    CHECK(cap->entries[0].msg == "w");
    CHECK(cap->entries[1].msg == "e");
    CHECK(cap->entries[2].msg == "c");
}

TEST_CASE("fmt-style formatting works", "[log]") {
    log_fixture fix;
    auto* cap = make_capture();

    info("hello {}", "world");
    info("{} + {} = {}", 1, 2, 3);

    REQUIRE(cap->entries.size() == 2);
    CHECK(cap->entries[0].msg == "hello world");
    CHECK(cap->entries[1].msg == "1 + 2 = 3");
}

TEST_CASE("clear_sinks stops output", "[log]") {
    log_fixture fix;
    auto* cap = make_capture();

    info("before");
    REQUIRE(cap->entries.size() == 1);

    clear_sinks();
    // cap is dangling after clear — no crash is the test
    info("after");
}

TEST_CASE("get_level and set_level round-trip", "[log]") {
    log_fixture fix;

    set_level(level::debug);
    CHECK(get_level() == level::debug);

    set_level(level::error);
    CHECK(get_level() == level::error);

    set_level(level::off);
    CHECK(get_level() == level::off);
}

TEST_CASE("level::off suppresses everything", "[log]") {
    log_fixture fix;
    auto* cap = make_capture();

    set_level(level::off);

    trace("t");
    debug("d");
    info("i");
    warn("w");
    error("e");
    critical("c");

    CHECK(cap->entries.empty());
}

TEST_CASE("trace and debug pass through when level is trace", "[log]") {
    log_fixture fix;
    auto* cap = make_capture();

    set_level(level::trace);

    trace("t");
    debug("d");

    REQUIRE(cap->entries.size() == 2);
    CHECK(cap->entries[0].msg == "t");
    CHECK(cap->entries[0].lvl == level::trace);
    CHECK(cap->entries[1].msg == "d");
    CHECK(cap->entries[1].lvl == level::debug);
}

TEST_CASE("file sink writes to disk", "[log]") {
    log_fixture fix;

    auto path = std::filesystem::temp_directory_path() / "collab_test_log.txt";
    std::filesystem::remove(path);

    add_sink(make_file_sink(path));
    info("line one");
    warn("line two");
    clear_sinks();

    std::ifstream in(path);
    REQUIRE(in.is_open());

    std::string content;
    std::string line;
    while (std::getline(in, line)) {
        if (!content.empty()) content += '\n';
        content += line;
    }

    CHECK(content.find("line one") != std::string::npos);
    CHECK(content.find("line two") != std::string::npos);

    in.close();
    std::filesystem::remove(path);
}

TEST_CASE("file sink appends across multiple sessions", "[log]") {
    log_fixture fix;

    auto path = std::filesystem::temp_directory_path() / "collab_test_log_append.txt";
    std::filesystem::remove(path);

    add_sink(make_file_sink(path));
    info("first");
    clear_sinks();

    add_sink(make_file_sink(path));
    info("second");
    clear_sinks();

    std::ifstream in(path);
    REQUIRE(in.is_open());

    std::string content;
    std::string line;
    while (std::getline(in, line)) {
        if (!content.empty()) content += '\n';
        content += line;
    }

    CHECK(content.find("first") != std::string::npos);
    CHECK(content.find("second") != std::string::npos);

    in.close();
    std::filesystem::remove(path);
}

TEST_CASE("stdout sink can be created and receives messages", "[log]") {
    log_fixture fix;

    add_sink(make_stdout_sink());
    set_level(level::warn);

    warn("stdout test warning");
    error("stdout test error");

    auto* cap = make_capture();

    info("filtered out");
    error("visible");

    REQUIRE(cap->entries.size() == 1);
    CHECK(cap->entries[0].msg == "visible");
}

TEST_CASE("stderr sink can be created and receives messages", "[log]") {
    log_fixture fix;

    add_sink(make_stderr_sink());
    set_level(level::warn);

    warn("stderr test warning");
    error("stderr test error");
    critical("stderr test critical");

    auto* cap = make_capture();

    info("filtered out");
    error("visible");

    REQUIRE(cap->entries.size() == 1);
    CHECK(cap->entries[0].msg == "visible");
}

TEST_CASE("multiple sinks receive the same message", "[log]") {
    log_fixture fix;
    auto* cap1 = make_capture();
    auto* cap2 = make_capture();

    info("broadcast");

    REQUIRE(cap1->entries.size() == 1);
    REQUIRE(cap2->entries.size() == 1);
    CHECK(cap1->entries[0].msg == "broadcast");
    CHECK(cap2->entries[0].msg == "broadcast");
}
