# collab-core 🏴‍☠️

Foundational C++23 library for the **Collab** stack. Ships as a single static library (`collab.core`) exposing one C++20 module (`collab.core`) with several partitions: errors, semantic versioning, structured logging, terminal styling, and a thread-safe signal/slot primitive.

Built with [xmake](https://xmake.io). Tested with [Catch2](https://github.com/catchorg/Catch2). C++23 modules required.

```cpp
import collab.core;

int main() {
    collab::log::add_sink(collab::log::make_stdout_color_sink());
    collab::log::info("collab.core {}", collab::core::version.to_string());
}
```

---

## Table of contents

- [Module layout](#module-layout)
- [`collab::core` — top-level](#collabcore--top-level)
- [`collab::core` — errors](#collabcore--errors)
- [`collab::core` — semver](#collabcore--semver)
- [`collab::log` — logging](#collablog--logging)
- [`collab::term` — terminal styling](#collabterm--terminal-styling)
- [`collab::core::Signal` — signal/slot](#collabcoresignal--signalslot)
- [Building](#building)
- [Testing](#testing)
- [License](#license)

---

## Module layout

One importable module: `collab.core`. One `import` brings in everything documented below across the `collab::core`, `collab::log`, and `collab::term` namespaces.

```cpp
import collab.core;
```

Public dependencies: [`fmt`](https://github.com/fmtlib/fmt) (re-exported as a package). Internal: [`spdlog`](https://github.com/gabime/spdlog), [`rang`](https://github.com/agauniyal/rang).

---

## `collab::core` — top-level

```cpp
namespace collab::core {
    inline constexpr semver version{1, 0, 0};
}
```

The library version itself, as a `semver` (see below).

---

## `collab::core` — errors

Tagged value-type for representing failures without throwing. Pair it with `std::expected<T, collab::core::Error>` (or your own `Result` alias) at API boundaries.

```cpp
struct Error {
    std::string message;
    std::string category = "general";
};
```

Predefined category tags (you can use any string, but these are the standard ones):

```cpp
namespace collab::core::error_category {
    general      // catch-all
    usage        // bad arguments / misuse
    not_found    // missing resource
    permission   // access denied
    network      // I/O over the wire
    timeout
    conflict     // version / state mismatch
    unavailable  // service down, retry-friendly
    internal     // invariant violation
}
```

```cpp
using namespace collab::core;
return Error{"config.yaml does not exist", std::string{error_category::not_found}};
```

---

## `collab::core` — semver

Semantic version per [SemVer 2.0](https://semver.org). Comparison ignores the `build` field (per §10).

```cpp
struct semver {
    int         major = 0;
    int         minor = 0;
    int         patch = 0;
    std::string pre_release;   // e.g. "rc.1"
    std::string build;         // e.g. "exp.sha.5114f85" — ignored in <=>

    std::string to_string() const;

    std::strong_ordering operator<=>(const semver&) const;
    bool                 operator==(const semver&) const;
};
```

```cpp
collab::core::semver v{1, 2, 0, "rc.1"};
assert(v < collab::core::semver{1, 2, 0});         // rc.1 precedes the release
assert(v.to_string() == "1.2.0-rc.1");
```

---

## `collab::log` — logging

Thin sink-routing layer over `spdlog` with both plain-string and `fmt`-style variadic overloads. Level filtering happens *before* `fmt::format` runs, so filtered messages cost nothing beyond an atomic load.

### Levels

```cpp
enum class collab::log::level { trace, debug, info, warn, error, critical, off };
```

### Configuration

```cpp
void  collab::log::set_level(level l);
level collab::log::get_level();

void  collab::log::add_sink(std::unique_ptr<sink> s);
void  collab::log::clear_sinks();
```

### Built-in sinks

```cpp
std::unique_ptr<sink> make_stdout_sink();
std::unique_ptr<sink> make_stdout_color_sink();
std::unique_ptr<sink> make_stderr_sink();
std::unique_ptr<sink> make_stderr_color_sink();
std::unique_ptr<sink> make_file_sink(std::filesystem::path path);
```

### Custom sinks

```cpp
class collab::log::sink {
public:
    virtual ~sink() = default;
    virtual void write(level lvl, std::string_view msg) = 0;
};
```

Implement and register with `add_sink(std::make_unique<MySink>(...))`.

### Logging

```cpp
// Plain string overloads
collab::log::trace   (std::string_view msg);
collab::log::debug   (std::string_view msg);
collab::log::info    (std::string_view msg);
collab::log::warn    (std::string_view msg);
collab::log::error   (std::string_view msg);
collab::log::critical(std::string_view msg);

// fmt-style variadic overloads
collab::log::info("connected to {} in {}ms", host, elapsed.count());
```

See [`docs/logging.md`](docs/logging.md) for additional notes.

---

## `collab::term` — terminal styling

Streaming manipulators for ANSI colors and styles. Output is automatically suppressed when stdout/stderr is not a TTY, when `NO_COLOR` is set, or when piped — handled internally by [`rang`](https://github.com/agauniyal/rang).

### Enums

```cpp
enum class collab::term::color {
    black, red, green, yellow, blue, magenta, cyan, gray, reset
};

enum class collab::term::style {
    bold, dim, italic, underline, blink, reversed, crossed, reset
};

std::ostream& operator<<(std::ostream&, color);
std::ostream& operator<<(std::ostream&, style);
```

### Convenience constants (for `using namespace collab::term;`)

```cpp
namespace collab::term::fg {
    black, red, green, yellow, blue, magenta, cyan, gray
}

reset_color, reset_style
bold, dim, italic, underline, blink, reversed, crossed
```

```cpp
using namespace collab::term;
std::cout << bold << fg::green << "ok " << reset_style << reset_color
          << "build complete\n";
```

---

## `collab::core::Signal` — signal/slot

Multi-subscriber, thread-safe signal. Subscriptions are RAII tokens that auto-disconnect on destruction. Convention (not enforced): only the owning class calls `emit()` — same rule as Qt, Boost.Signals2, sigc++.

### `Signal<Args...>`

```cpp
template <typename... Args>
class Signal {
public:
    using Handler = std::function<void(Args...)>;

    Signal();
    Signal(const Signal&)            = delete;
    Signal(Signal&&)                 = delete;
    Signal& operator=(const Signal&) = delete;
    Signal& operator=(Signal&&)      = delete;

    [[nodiscard]] Subscription connect(Handler handler);
    void                       emit(Args... args);
    std::size_t                subscriber_count() const;
};
```

### `Subscription`

Move-only RAII token. Disconnect is automatic on destruction; calling `disconnect()` is also fine.

```cpp
class Subscription {
public:
    Subscription() noexcept = default;
    Subscription(Subscription&&) noexcept;
    Subscription& operator=(Subscription&&) noexcept;
    ~Subscription();

    void disconnect() noexcept;
    bool connected()  const noexcept;
};
```

### Threading contract

- `connect()`, `emit()`, `disconnect()`, and `subscriber_count()` are all safe to call concurrently from any thread on the same `Signal`.
- `emit()` does **not** hold the internal lock while invoking handlers — it snapshots the slot list under a shared lock, releases, then iterates. Reentrant and recursive `emit()` are deadlock-free.
- A handler may freely `connect()`, `disconnect()`, or `emit()` (including on the same `Signal`).
- Disconnects during an in-flight `emit()` affect *subsequent* emits, not the current one. Handlers already snapshotted still fire.
- A `Subscription` may safely outlive its `Signal`. Disconnect becomes a no-op.

### Example

```cpp
collab::core::Signal<int, std::string> changed;

auto sub = changed.connect([](int code, std::string_view msg) {
    collab::log::info("changed: {} ({})", msg, code);
});

changed.emit(42, "ready");

// Disconnect explicitly, or just let `sub` go out of scope.
sub.disconnect();
```

---

## Building

C++23 with module support is required. See the comment block at the top of [`xmake.lua`](xmake.lua) for the canonical per-toolchain config commands (GCC 15, Clang + libc++, macOS LLVM, with optional ThreadSanitizer).

Quick start:

```sh
xmake f -y                # configure (defaults: release mode, build_tests=on)
xmake                     # build everything
xmake run tests-collab.core   # run the test binary directly
```

Disable tests:

```sh
xmake f -y --build_tests=n
```

---

## Testing

Tests live under `lib/collab.core/tests/` and run via `xmake test` (Catch2, `--durations yes`).

```sh
xmake test
```

---

## License

[BSD Zero Clause](LICENSE) (SPDX: `0BSD`).
