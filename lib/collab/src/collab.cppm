module;

#include <filesystem>
#include <memory>
#include <mutex>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include <collab/error.hpp>
#include <collab/identifier.hpp>
#include <collab/manifest.hpp>
#include <collab/publisher.hpp>
#include <collab/semver.hpp>

export module collab;

export import :term;

// ── Re-export the value types from collab-hpp ────────────────────────────

namespace collab {
    export using ::collab::error;
    export using ::collab::identifier;
    export using ::collab::manifest;
    export using ::collab::publisher;
    export using ::collab::semver;
    export using ::collab::subscription;
}

// ── Log API: module-local declarations + definitions ────────────────────
//
// Because MSVC's IFC consumer ICEs on `export using ::collab::log::*` of
// inline functions, and a clean dual-source pattern requires either upstream
// changes to collab-hpp or a working `export using` path that doesn't exist
// today on this toolchain, the log surface is declared fresh inside the
// module. The state singleton here is module-local; a program that mixes
// `import collab;` and `#include <collab/log.hpp>` ends up with two
// independent sink lists — pick one path per program.

export namespace collab::log {

enum class level { trace, debug, info, warn, error, critical, off };

class sink {
public:
    virtual ~sink() = default;
    virtual void write(level lvl,
                       const collab::identifier* id,
                       std::string_view msg) = 0;
};

void set_level(level l);
level get_level();
void add_sink(std::unique_ptr<sink> snk);
void clear_sinks();
void log_message(level lvl, const collab::identifier* id, std::string_view msg);

void trace   (std::string_view msg);
void debug   (std::string_view msg);
void info    (std::string_view msg);
void warn    (std::string_view msg);
void error   (std::string_view msg);
void critical(std::string_view msg);

void trace_with   (const collab::identifier& id, std::string_view msg);
void debug_with   (const collab::identifier& id, std::string_view msg);
void info_with    (const collab::identifier& id, std::string_view msg);
void warn_with    (const collab::identifier& id, std::string_view msg);
void error_with   (const collab::identifier& id, std::string_view msg);
void critical_with(const collab::identifier& id, std::string_view msg);

// fmt-style variadic overloads — inline, instantiated in consumer TUs.

template<typename... Args>
void trace(fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::trace)
        trace(std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void debug(fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::debug)
        debug(std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void info(fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::info)
        info(std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void warn(fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::warn)
        warn(std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void error(fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::error)
        error(std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void critical(fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::critical)
        critical(std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}

template<typename... Args>
void trace_with(const collab::identifier& id, fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::trace)
        trace_with(id, std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void debug_with(const collab::identifier& id, fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::debug)
        debug_with(id, std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void info_with(const collab::identifier& id, fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::info)
        info_with(id, std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void warn_with(const collab::identifier& id, fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::warn)
        warn_with(id, std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void error_with(const collab::identifier& id, fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::error)
        error_with(id, std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}
template<typename... Args>
void critical_with(const collab::identifier& id, fmt::format_string<Args...> fs, Args&&... args) {
    if (get_level() <= level::critical)
        critical_with(id, std::string_view(fmt::format(fs, std::forward<Args>(args)...)));
}

template<const collab::identifier& I>
struct logger {
    template<typename... Args>
    static void trace(fmt::format_string<Args...> fs, Args&&... args) {
        trace_with(I, fs, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void debug(fmt::format_string<Args...> fs, Args&&... args) {
        debug_with(I, fs, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void info(fmt::format_string<Args...> fs, Args&&... args) {
        info_with(I, fs, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void warn(fmt::format_string<Args...> fs, Args&&... args) {
        warn_with(I, fs, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void error(fmt::format_string<Args...> fs, Args&&... args) {
        error_with(I, fs, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void critical(fmt::format_string<Args...> fs, Args&&... args) {
        critical_with(I, fs, std::forward<Args>(args)...);
    }

    static void trace   (std::string_view msg) { trace_with   (I, msg); }
    static void debug   (std::string_view msg) { debug_with   (I, msg); }
    static void info    (std::string_view msg) { info_with    (I, msg); }
    static void warn    (std::string_view msg) { warn_with    (I, msg); }
    static void error   (std::string_view msg) { error_with   (I, msg); }
    static void critical(std::string_view msg) { critical_with(I, msg); }
};

// ── spdlog-backed sink factories ─────────────────────────────────────────

std::unique_ptr<sink> make_stdout_sink();
std::unique_ptr<sink> make_stdout_color_sink();
std::unique_ptr<sink> make_stderr_sink();
std::unique_ptr<sink> make_stderr_color_sink();
std::unique_ptr<sink> make_file_sink(std::filesystem::path path);

}  // namespace collab::log
