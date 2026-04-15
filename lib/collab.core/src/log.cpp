module;

#include <atomic>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

module collab.core;

namespace collab::log {

namespace {

    spdlog::level::level_enum to_spdlog_level(level l) {
        switch (l) {
            case level::trace:    return spdlog::level::trace;
            case level::debug:    return spdlog::level::debug;
            case level::info:     return spdlog::level::info;
            case level::warn:     return spdlog::level::warn;
            case level::error:    return spdlog::level::err;
            case level::critical: return spdlog::level::critical;
            case level::off:      return spdlog::level::off;
        }
        return spdlog::level::info;
    }

    constexpr auto logger_prefix_stdout = "collab_stdout_";
    constexpr auto logger_prefix_stderr = "collab_stderr_";
    constexpr auto logger_prefix_file   = "collab_file_";
    constexpr auto log_pattern          = "%v";

    struct log_state {
        level                               current_level = level::info;
        std::vector<std::unique_ptr<sink>>   sinks;
        std::vector<std::string>             logger_names;
        std::mutex                           mtx;
    };

    log_state& state() {
        static log_state s;
        return s;
    }

    void register_logger(std::shared_ptr<spdlog::logger>& logger, const std::string& name) {
        logger->set_level(spdlog::level::trace);  // collab layer is the sole gatekeeper
        logger->set_pattern(log_pattern);
        auto& s = state();
        std::lock_guard lock(s.mtx);
        s.logger_names.push_back(name);
    }

    // ── spdlog-backed stdout sinks ──────────────────────────────────

    class stdout_sink final : public sink {
    public:
        stdout_sink() {
            auto name = logger_prefix_stdout + std::to_string(++counter_);
            logger_ = spdlog::stdout_logger_mt(name);
            register_logger(logger_, name);
        }

        void write(level lvl, std::string_view msg) override {
            logger_->log(to_spdlog_level(lvl), "{}", msg);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
        static inline std::atomic<int> counter_{0};
    };

    class stdout_color_sink final : public sink {
    public:
        stdout_color_sink() {
            auto name = logger_prefix_stdout + std::to_string(++counter_);
            logger_ = spdlog::stdout_color_mt(name);
            register_logger(logger_, name);
        }

        void write(level lvl, std::string_view msg) override {
            logger_->log(to_spdlog_level(lvl), "{}", msg);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
        static inline std::atomic<int> counter_{0};
    };

    // ── spdlog-backed stderr sinks ──────────────────────────────────

    class stderr_sink final : public sink {
    public:
        stderr_sink() {
            auto name = logger_prefix_stderr + std::to_string(++counter_);
            logger_ = spdlog::stderr_logger_mt(name);
            register_logger(logger_, name);
        }

        void write(level lvl, std::string_view msg) override {
            logger_->log(to_spdlog_level(lvl), "{}", msg);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
        static inline std::atomic<int> counter_{0};
    };

    class stderr_color_sink final : public sink {
    public:
        stderr_color_sink() {
            auto name = logger_prefix_stderr + std::to_string(++counter_);
            logger_ = spdlog::stderr_color_mt(name);
            register_logger(logger_, name);
        }

        void write(level lvl, std::string_view msg) override {
            logger_->log(to_spdlog_level(lvl), "{}", msg);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
        static inline std::atomic<int> counter_{0};
    };

    // ── spdlog-backed file sink ─────────────────────────────────────

    class file_sink final : public sink {
    public:
        explicit file_sink(std::filesystem::path path) {
            auto name = logger_prefix_file + std::to_string(++counter_);
            logger_ = spdlog::basic_logger_mt(name, path.string(), false);
            register_logger(logger_, name);
            logger_->flush_on(spdlog::level::trace);
        }

        void write(level lvl, std::string_view msg) override {
            logger_->log(to_spdlog_level(lvl), "{}", msg);
        }

    private:
        std::shared_ptr<spdlog::logger> logger_;
        static inline std::atomic<int> counter_{0};
    };

}  // namespace

void set_level(level l) {
    auto& s = state();
    std::lock_guard lock(s.mtx);
    s.current_level = l;
}

level get_level() {
    auto& s = state();
    std::lock_guard lock(s.mtx);
    return s.current_level;
}

void add_sink(std::unique_ptr<sink> snk) {
    auto& s = state();
    std::lock_guard lock(s.mtx);
    s.sinks.push_back(std::move(snk));
}

void clear_sinks() {
    auto& s = state();
    std::lock_guard lock(s.mtx);
    s.sinks.clear();
    for (auto& name : s.logger_names)
        spdlog::drop(name);
    s.logger_names.clear();
}

void log_message(level lvl, std::string_view msg) {
    auto& s = state();
    std::lock_guard lock(s.mtx);
    if (lvl < s.current_level) return;
    for (auto& snk : s.sinks)
        snk->write(lvl, msg);
}

void trace(std::string_view msg)    { log_message(level::trace, msg); }
void debug(std::string_view msg)    { log_message(level::debug, msg); }
void info(std::string_view msg)     { log_message(level::info, msg); }
void warn(std::string_view msg)     { log_message(level::warn, msg); }
void error(std::string_view msg)    { log_message(level::error, msg); }
void critical(std::string_view msg) { log_message(level::critical, msg); }

std::unique_ptr<sink> make_stdout_sink() {
    return std::make_unique<stdout_sink>();
}

std::unique_ptr<sink> make_stdout_color_sink() {
    return std::make_unique<stdout_color_sink>();
}

std::unique_ptr<sink> make_stderr_sink() {
    return std::make_unique<stderr_sink>();
}

std::unique_ptr<sink> make_stderr_color_sink() {
    return std::make_unique<stderr_color_sink>();
}

std::unique_ptr<sink> make_file_sink(std::filesystem::path path) {
    return std::make_unique<file_sink>(std::move(path));
}

}  // namespace collab::log
