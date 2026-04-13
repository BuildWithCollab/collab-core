# Logging & Terminal Colors

`collab.core` provides two related APIs: `collab::log` for structured logging with pluggable sinks, and `collab::term` for terminal color/style output.

Both are available via `import collab.core;`.

---

## collab::log

### Quick start

```cpp
import collab.core;

using namespace collab::log;

int main() {
    add_sink(make_stderr_sink());   // colored output to stderr
    set_level(level::debug);

    info("server starting on port {}", 8080);
    debug("loading config from {}", path);
    warn("cache miss for key {}", key);
    error("connection failed: {}", err.message);
}
```

### No sinks = silent

Libraries can log freely without worrying about whether anyone is listening. If no sinks are configured, all log calls are no-ops (the function call happens, but nothing is written). A CLI or app configures sinks at startup, and everything lights up.

```cpp
// In a library — just log, don't configure sinks
void connect(std::string_view host) {
    info("connecting to {}", host);
    if (!reachable(host))
        warn("host {} unreachable, retrying", host);
}
```

### Log levels

```cpp
enum class level { trace, debug, info, warn, error, critical, off };
```

Default level is `info`. Set it with `set_level(level::debug)`, query with `get_level()`.

### Log functions

Each level has two overloads:

```cpp
// Plain message
info("server started");

// fmt-style formatting (avoids formatting cost if the level is filtered out)
info("listening on {}:{}", host, port);
```

All six levels: `trace`, `debug`, `info`, `warn`, `error`, `critical`.

### Sinks

Three built-in sink factories:

| Factory | Output | Colors |
|---------|--------|--------|
| `make_stdout_sink()` | stdout | Yes (via spdlog) |
| `make_stderr_sink()` | stderr | Yes (via spdlog) |
| `make_file_sink(path)` | file (appends) | No |

```cpp
add_sink(make_stderr_sink());
add_sink(make_file_sink("/var/log/myapp.log"));
```

Multiple sinks can be active simultaneously. Each receives every message that passes the level filter.

`clear_sinks()` removes all sinks and releases their resources.

### Custom sinks

Subclass `collab::log::sink` and implement `write()`:

```cpp
class my_sink final : public collab::log::sink {
public:
    void write(collab::log::level lvl, std::string_view msg) override {
        // send to your logging backend, database, network, etc.
    }
};

add_sink(std::make_unique<my_sink>());
```

### Architecture

spdlog is the I/O backend but is **never exposed** to consumers. The public API is `collab::log::*` only. spdlog handles file writing, color codes, buffering, and flushing. It could be swapped for any other backend by changing `log.cpp` — no consumer code would change.

The collab layer is the sole gatekeeper for level filtering. spdlog loggers are set to `trace` internally so they pass everything through.

### Thread safety

All log functions, `set_level`, `get_level`, `add_sink`, and `clear_sinks` are thread-safe. A single mutex guards the sink list and level state. Sink construction (via `make_*_sink()`) is also thread-safe.

The fmt-style template overloads check the level before formatting to avoid unnecessary `fmt::format` work. There is a benign TOCTOU window where the level could change between the check and the actual dispatch, but this is acceptable for logging — a message may slip through or be dropped during a concurrent level change.

---

## collab::term

### Quick start

```cpp
import collab.core;

using namespace collab::term;

std::cerr << fg::red << "error: " << reset_color << "something broke\n";
std::cout << bold << fg::yellow << "warning: " << reset_style << reset_color << "watch out\n";
```

### Colors

Available via the `fg::` namespace:

```
fg::black   fg::red     fg::green   fg::yellow
fg::blue    fg::magenta fg::cyan    fg::gray
```

Reset with `reset_color`.

### Styles

Available as top-level constants:

```
bold   dim   italic   underline   blink   reversed   crossed
```

Reset with `reset_style`.

### How it works

`collab::term` wraps [rang](https://github.com/agauniyal/rang) behind its own enums. rang is a private implementation detail — consumers never see `rang::` types.

rang automatically handles:
- **`NO_COLOR` env var** — per [no-color.org](https://no-color.org), colors are suppressed when this is set
- **TTY detection** — no ANSI codes when output is piped or redirected
- **Windows console** — uses the appropriate Windows API when needed

### Usage with collab::log

`collab::term` and `collab::log` are independent. The log sinks handle their own coloring (spdlog's color sinks). `collab::term` is for direct terminal output — CLI status messages, prompts, formatted tables, etc.

---

## Dependencies

| Package | Public? | Role |
|---------|---------|------|
| `fmt` | **Yes** | String formatting for log messages and general use |
| `spdlog` | **No** | I/O backend for log sinks (hidden behind `collab::log`) |
| `rang` | **No** | ANSI color/style codes (hidden behind `collab::term`) |

---

## Module structure

Both APIs live as partitions inside `collab.core`:

```
lib/collab.core/src/
    collab.core.cppm    # export import :log; export import :term;
    log.cppm            # public API declarations
    log.cpp             # spdlog-backed implementation
    term.cppm           # public API declarations
    term.cpp            # rang-backed implementation
```

`import collab.core;` gives you everything.
