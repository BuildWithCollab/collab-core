# `import collab;` 🏴‍☠️

The static-link dependency floor for the **Collab** stack. Every non-header-only Collab project pulls this in to get a guaranteed baseline: spdlog-backed log sinks today, more as the floor grows.

Built on [`collab-hpp`](https://github.com/BuildWithCollab/collab.hpp) — pulled in transitively, so its header-only surface is along for the ride. See that README for the foundation; this one covers what `import collab;` adds.

Requires a C++23 toolchain with module support.

---

## Getting started

```cpp
import collab;
```

That's it — `import collab;` brings in the spdlog-backed sink factories and re-exports everything `collab-hpp` provides through its headers. Apps wire sinks up at startup; libraries just log.

---

## Sinks

The application installs sinks at startup and decides where output ends up:

```cpp
int main() {
    collab::log::add_sink(collab::log::make_stdout_color_sink());
    collab::log::add_sink(collab::log::make_file_sink("app.log"));
    collab::log::set_level(collab::log::level::debug);
    // ...
}
```

Console sinks render the caller's identifier using its display name — humans reading a terminal want `[Collab Net]` not `[com.mrowrpurr.collab-net]`. File sinks invert that: they render the bundle ID, because grepping logs after the fact wants the stable reverse-DNS form.

```
[Collab Net] connecting to example.com:443        ← stdout / stderr
[com.mrowrpurr.collab-net] connecting to ...      ← app.log
```

Color variants use Win32 console attributes on Windows and ANSI escapes elsewhere. Both are no-ops when the stream isn't a TTY.

---

## Terminal styling

Streaming manipulators for ANSI colors and styles, scoped under `collab::term`. Output is automatically suppressed when stdout/stderr isn't a TTY, when `NO_COLOR` is set, or when piped — the same code is correct in a terminal and in a logged-to-file build run.

```cpp
using namespace collab::term;

std::cout << bold << fg::green << "ok " << reset_style << reset_color
          << "build complete\n";
```

Foreground colors live under `collab::term::fg::` — `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `gray`. Styles live directly under `collab::term::` — `bold`, `dim`, `italic`, `underline`, `blink`, `reversed`, `crossed`. Reset with `reset_color` (back to the terminal default) and `reset_style` (clears bold/italic/etc).

---

## License

[BSD Zero Clause](LICENSE) (SPDX: `0BSD`).
