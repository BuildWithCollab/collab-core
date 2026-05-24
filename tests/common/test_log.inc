// Log tests — header-only-compatible. Uses a custom sink subclass instead
// of the spdlog factories (which are only available with the static lib).

namespace test_log_detail {

struct capturing_sink final : public collab::log::sink {
    struct entry {
        collab::log::level lvl;
        const collab::identifier* id;
        std::string msg;
    };
    std::vector<entry> entries;

    void write(collab::log::level lvl,
               const collab::identifier* id,
               std::string_view msg) override {
        entries.push_back({lvl, id, std::string(msg)});
    }
};

// Tests share global state (level, sinks). Reset between cases.
inline capturing_sink& install_capturing_sink() {
    collab::log::clear_sinks();
    collab::log::set_level(collab::log::level::trace);
    auto owned = std::make_unique<capturing_sink>();
    auto* raw  = owned.get();
    collab::log::add_sink(std::move(owned));
    return *raw;
}

}  // namespace test_log_detail

TEST_CASE("log: set/get level round-trip", "[log]") {
    collab::log::clear_sinks();
    collab::log::set_level(collab::log::level::warn);
    REQUIRE(collab::log::get_level() == collab::log::level::warn);
    collab::log::set_level(collab::log::level::trace);
    REQUIRE(collab::log::get_level() == collab::log::level::trace);
    collab::log::set_level(collab::log::level::info);
}

TEST_CASE("log: untagged free functions reach sinks", "[log]") {
    auto& sink = test_log_detail::install_capturing_sink();
    collab::log::info("hello");
    collab::log::warn("careful");
    REQUIRE(sink.entries.size() == 2);
    REQUIRE(sink.entries[0].lvl == collab::log::level::info);
    REQUIRE(sink.entries[0].msg == "hello");
    REQUIRE(sink.entries[0].id == nullptr);
    REQUIRE(sink.entries[1].lvl == collab::log::level::warn);
    REQUIRE(sink.entries[1].msg == "careful");
}

TEST_CASE("log: tagged free functions carry the identifier pointer", "[log]") {
    auto& sink = test_log_detail::install_capturing_sink();
    collab::identifier id{
        .app_id = "test", .app_name = "Test", .org_id = "x", .org_name = "X", .tld = "com",
    };
    collab::log::info_with(id, "tagged");
    REQUIRE(sink.entries.size() == 1);
    REQUIRE(sink.entries[0].id == &id);
    REQUIRE(sink.entries[0].msg == "tagged");
}

TEST_CASE("log: level filtering drops messages below threshold", "[log]") {
    auto& sink = test_log_detail::install_capturing_sink();
    collab::log::set_level(collab::log::level::warn);
    collab::log::trace("dropped");
    collab::log::debug("dropped");
    collab::log::info("dropped");
    collab::log::warn("kept");
    collab::log::error("kept");
    REQUIRE(sink.entries.size() == 2);
    REQUIRE(sink.entries[0].msg == "kept");
    REQUIRE(sink.entries[1].msg == "kept");
    collab::log::set_level(collab::log::level::trace);
}

TEST_CASE("log: fmt-style formatting", "[log]") {
    auto& sink = test_log_detail::install_capturing_sink();
    collab::log::info("counter={} name={}", 42, "neo");
    REQUIRE(sink.entries.size() == 1);
    REQUIRE(sink.entries[0].msg == "counter=42 name=neo");
}

TEST_CASE("log: multiple sinks all receive each message", "[log]") {
    collab::log::clear_sinks();
    collab::log::set_level(collab::log::level::trace);

    auto a = std::make_unique<test_log_detail::capturing_sink>();
    auto b = std::make_unique<test_log_detail::capturing_sink>();
    auto* a_raw = a.get();
    auto* b_raw = b.get();
    collab::log::add_sink(std::move(a));
    collab::log::add_sink(std::move(b));

    collab::log::info("broadcast");
    REQUIRE(a_raw->entries.size() == 1);
    REQUIRE(b_raw->entries.size() == 1);
    REQUIRE(a_raw->entries[0].msg == "broadcast");
    REQUIRE(b_raw->entries[0].msg == "broadcast");

    collab::log::clear_sinks();
}

namespace test_log_detail {
// Module-scope identifier for the logger<I> NTTP test.
inline const collab::identifier lib_ident{
    .app_id = "lib-x", .app_name = "Lib X",
    .org_id = "org", .org_name = "Org",
    .tld    = "io",
};
using lib_log = collab::log::logger<lib_ident>;
}  // namespace test_log_detail

TEST_CASE("log: logger<I> binds an identifier at compile time", "[log]") {
    auto& sink = test_log_detail::install_capturing_sink();
    test_log_detail::lib_log::info("ready on :{}", 8080);
    test_log_detail::lib_log::warn("plain");
    REQUIRE(sink.entries.size() == 2);
    REQUIRE(sink.entries[0].id == &test_log_detail::lib_ident);
    REQUIRE(sink.entries[0].msg == "ready on :8080");
    REQUIRE(sink.entries[1].id == &test_log_detail::lib_ident);
    REQUIRE(sink.entries[1].msg == "plain");
    collab::log::clear_sinks();
}
