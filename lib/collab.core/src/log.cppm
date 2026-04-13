module;

#include <filesystem>
#include <memory>
#include <string_view>
#include <fmt/format.h>

export module collab.core:log;

export namespace collab::log {

enum class level { trace, debug, info, warn, error, critical, off };

class sink {
public:
    virtual ~sink() = default;
    virtual void write(level lvl, std::string_view msg) = 0;
};

void set_level(level l);
level get_level();

void add_sink(std::unique_ptr<sink> s);
void clear_sinks();

void log_message(level lvl, std::string_view msg);

std::unique_ptr<sink> make_stdout_sink();
std::unique_ptr<sink> make_stderr_sink();
std::unique_ptr<sink> make_file_sink(std::filesystem::path path);

// Plain string_view overloads
void trace(std::string_view msg);
void debug(std::string_view msg);
void info(std::string_view msg);
void warn(std::string_view msg);
void error(std::string_view msg);
void critical(std::string_view msg);

// fmt-style variadic overloads.
// The level check here avoids paying fmt::format cost for filtered messages.
// A TOCTOU exists (level could change between the check and log_message),
// but that's benign for logging — a message may slip through or be dropped
// during a concurrent level change, which is acceptable.
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

}  // namespace collab::log
