// Log sink-factory tests — require the static collab library (spdlog impl).
// The standard headers this file needs (filesystem, fstream, sstream, cstdio)
// must be included by the enclosing TU BEFORE `import collab;` to avoid MSVC
// duplicate-template-specialization errors when the same std headers appear
// in both the module's GMF and the consumer's TU.

TEST_CASE("log[ext]: make_stdout_sink constructs without crashing", "[log][ext]") {
    collab::log::clear_sinks();
    auto s = collab::log::make_stdout_sink();
    REQUIRE(s != nullptr);
    collab::log::add_sink(std::move(s));
    collab::log::info("stdout sink alive");
    collab::log::clear_sinks();
}

TEST_CASE("log[ext]: stderr/stdout color variants construct", "[log][ext]") {
    REQUIRE(collab::log::make_stdout_color_sink() != nullptr);
    REQUIRE(collab::log::make_stderr_sink() != nullptr);
    REQUIRE(collab::log::make_stderr_color_sink() != nullptr);
}

TEST_CASE("log[ext]: file sink writes messages to disk", "[log][ext]") {
    namespace fs = std::filesystem;
    auto path = fs::temp_directory_path() / "collab_test_log_sink.log";
    std::error_code ec;
    fs::remove(path, ec);

    collab::log::clear_sinks();
    collab::log::set_level(collab::log::level::trace);
    collab::log::add_sink(collab::log::make_file_sink(path));

    collab::log::info("first");
    collab::log::warn("second");

    collab::log::clear_sinks();  // drop the sink so the file is flushed/closed

    std::ifstream in(path);
    REQUIRE(in.good());
    std::stringstream buf;
    buf << in.rdbuf();
    const auto contents = buf.str();
    REQUIRE(contents.find("first")  != std::string::npos);
    REQUIRE(contents.find("second") != std::string::npos);

    fs::remove(path, ec);
}

TEST_CASE("log[ext]: file sink with tagged messages emits bundle_id prefix", "[log][ext]") {
    namespace fs = std::filesystem;
    auto path = fs::temp_directory_path() / "collab_test_log_sink_tagged.log";
    std::error_code ec;
    fs::remove(path, ec);

    collab::log::clear_sinks();
    collab::log::set_level(collab::log::level::trace);
    collab::log::add_sink(collab::log::make_file_sink(path));

    collab::identifier id{
        .app_id   = "collab-net",
        .app_name = "Collab Net",
        .org_id   = "mrowrpurr",
        .org_name = "Mrowr Purr",
        .tld      = "com",
    };
    collab::log::info_with(id, "tagged-to-file");
    collab::log::clear_sinks();

    std::ifstream in(path);
    REQUIRE(in.good());
    std::stringstream buf;
    buf << in.rdbuf();
    const auto contents = buf.str();
    REQUIRE(contents.find("[com.mrowrpurr.collab-net] tagged-to-file") != std::string::npos);

    fs::remove(path, ec);
}
