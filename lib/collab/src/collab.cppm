module;

#include <filesystem>
#include <memory>

export module collab;

export import :term;

// Single source of truth: re-export collab-hpp's full surface as a header unit.
// Consumers of `import collab;` reach `collab::identifier`, `collab::log::set_level`,
// `collab::log::logger<I>`, fmt-style templates — all from collab-hpp, no duplication.
export import <collab.hpp>;

// spdlog-backed sink factories — the only surface this static library adds.
// Definitions in lib/collab/src/log.cpp.
export namespace collab::log {

std::unique_ptr<sink> make_stdout_sink();
std::unique_ptr<sink> make_stdout_color_sink();
std::unique_ptr<sink> make_stderr_sink();
std::unique_ptr<sink> make_stderr_color_sink();
std::unique_ptr<sink> make_file_sink(std::filesystem::path path);

}  // namespace collab::log
