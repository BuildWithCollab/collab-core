#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>

import collab;

using namespace collab::log;

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

TEST_CASE("logging with no sinks does not crash", "[log]") {
    log_fixture fix;
    info("hello");
    warn("warning");
    error("error");
    critical("critical");
}

TEST_CASE("level filtering works", "[log]") {
    log_fixture fix;

    struct capture : sink {
        std::vector<std::string> msgs;
        void write(level, const collab::identifier*, std::string_view m) override {
            msgs.emplace_back(m);
        }
    };
    auto owned = std::make_unique<capture>();
    auto* cap  = owned.get();
    add_sink(std::move(owned));

    set_level(level::warn);

    trace("t"); debug("d"); info("i");
    warn("w"); error("e"); critical("c");

    REQUIRE(cap->msgs.size() == 3);
    CHECK(cap->msgs[0] == "w");
    CHECK(cap->msgs[1] == "e");
    CHECK(cap->msgs[2] == "c");
}

TEST_CASE("fmt-style formatting works", "[log]") {
    log_fixture fix;
    struct capture : sink {
        std::vector<std::string> msgs;
        void write(level, const collab::identifier*, std::string_view m) override {
            msgs.emplace_back(m);
        }
    };
    auto owned = std::make_unique<capture>();
    auto* cap  = owned.get();
    add_sink(std::move(owned));

    info("hello {}", "world");
    info("{} + {} = {}", 1, 2, 3);

    REQUIRE(cap->msgs.size() == 2);
    CHECK(cap->msgs[0] == "hello world");
    CHECK(cap->msgs[1] == "1 + 2 = 3");
}

TEST_CASE("logger<I> dispatches with bound identifier", "[log][logger]") {
    log_fixture fix;

    struct capture : sink {
        std::vector<std::pair<std::string, std::string>> entries;
        void write(level, const collab::identifier* id, std::string_view m) override {
            entries.emplace_back(id ? std::string(id->app_id) : std::string{},
                                 std::string(m));
        }
    };
    auto owned = std::make_unique<capture>();
    auto* cap  = owned.get();
    add_sink(std::move(owned));

    static constexpr collab::identifier id_a{
        .app_id   = "lib-a",
        .app_name = "Lib A",
        .org_id   = "purr",
        .org_name = "Purr",
        .tld      = "com",
    };
    using log_a = collab::log::logger<id_a>;

    log_a::info("from a");
    log_a::error("err {}", 42);

    REQUIRE(cap->entries.size() == 2);
    CHECK(cap->entries[0].first  == "lib-a");
    CHECK(cap->entries[0].second == "from a");
    CHECK(cap->entries[1].first  == "lib-a");
    CHECK(cap->entries[1].second == "err 42");
}

// ── spdlog sink integration ─────────────────────────────────────────────

TEST_CASE("stdout sink can be created", "[log][sink]") {
    log_fixture fix;
    add_sink(make_stdout_sink());
    set_level(level::warn);
    warn("stdout test warning");
    error("stdout test error");
}

TEST_CASE("stdout color sink can be created", "[log][sink]") {
    log_fixture fix;
    add_sink(make_stdout_color_sink());
    set_level(level::warn);
    warn("stdout color warning");
    error("stdout color error");
}

TEST_CASE("stderr sink can be created", "[log][sink]") {
    log_fixture fix;
    add_sink(make_stderr_sink());
    set_level(level::warn);
    warn("stderr test warning");
    error("stderr test error");
    critical("stderr test critical");
}

TEST_CASE("stderr color sink can be created", "[log][sink]") {
    log_fixture fix;
    add_sink(make_stderr_color_sink());
    set_level(level::warn);
    warn("stderr color warning");
    error("stderr color error");
    critical("stderr color critical");
}

TEST_CASE("file sink writes to disk", "[log][sink]") {
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

TEST_CASE("file sink renders identifier with bundle id", "[log][sink][identifier]") {
    log_fixture fix;

    static const collab::identifier id{
        .app_id   = "lib-x",
        .app_name = "Lib X",
        .org_id   = "purr",
        .org_name = "Purr",
        .tld      = "com",
    };

    auto path = std::filesystem::temp_directory_path() / "collab_test_log_id.txt";
    std::filesystem::remove(path);

    add_sink(make_file_sink(path));
    info_with(id, "tagged");
    clear_sinks();

    std::ifstream in(path);
    REQUIRE(in.is_open());
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    CHECK(content.find("[com.purr.lib-x] tagged") != std::string::npos);

    std::filesystem::remove(path);
}
