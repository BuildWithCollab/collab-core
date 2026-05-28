# 🤖 CODEGEN.md

C++ module codegen for **dual-mode static libraries** — libraries that can be reached from the same translation unit through both `#include <name.hpp>` and `import name;` without ODR splits or compiler crashes.

You point the script at a directory of canonical inline headers, give it an output directory and a module name, and it emits the boilerplate that makes both consumption paths work.

---

## What "dual-mode" means

Two ways for a downstream TU to reach a C++ library:

1. **Header-only:** `#include <name/area.hpp>` — the consumer's compiler sees every inline body and instantiates them in-line. No link dependency on a built library.
2. **Modules:** `import name;` — the consumer pulls in a pre-built BMI. Inline bodies are link-time symbols in `name.lib` / `libname.a`.

A library is **dual-mode** when both paths work *at the same time, in the same TU*. That's harder than it sounds:

- **MSVC IFC consumer ICE.** If a plain `inline` function body lands in a module's BMI, downstream `import` consumers crash at compile time. The fix is to never let plain inline bodies enter the BMI in the first place.
- **ODR identity.** Declarations made inside a module's purview (after `export module name:area;`) attach to *module `name`*. Declarations brought in via `#include` in the global module fragment (GMF) attach to the *global module*. A dual-mode TU must reach the **same** entity through both paths. Module-purview definitions versus global-module declarations of the same name are two different entities — link-time disaster.

Both constraints push the same direction: declarations attach to the **global module** (via GMF `#include`), the module partition only **re-exports** them with `using ::ns::name;`, and inline function bodies are emitted as out-of-line COMDAT symbols by a separate impl unit so `import` consumers can link to them.

That's the architecture the codegen produces.

---

## The four files per area

An **area** is one canonical inline header. For each canonical at `<--include>/<rel>.hpp`, the generator emits a sibling triple at `<--src>/<rel>.*`:

| File | What it is |
|---|---|
| `<rel>.decls.hpp` | The canonical with plain `inline` function bodies stripped to declarations and inline variables turned into `extern`. Build-internal — only the sibling `.cppm` includes it. |
| `<rel>.cppm` | Module partition: `module;` + `#include "<basename>.decls.hpp"` (GMF) + `export module <name>:<flat-rel>;` + `using ::ns::name;` re-exports for every top-level entity. |
| `<rel>_impl.cpp` | Module impl unit: `module;` + `#include <<name>/<rel>.hpp>` (the canonical, with bodies) + `module <name>;` + a force-emission array that address-takes every plain inline function so the linker keeps the COMDAT symbols. |

You **also** hand-write three things the generator never touches:

| File | What it is |
|---|---|
| `<name>.hpp` (umbrella) | One `#include <<name>/<rel>.hpp>` per canonical. The entry point for header-only consumers. |
| `<name>.cppm` (primary) | `export module <name>;` + `export import :<flat-rel>;` per partition. The entry point for `import` consumers. |
| `<rel>_native.cpp` *(optional)* | For non-inline functions the canonical only *declares* (e.g., factories that link a private external library). Plain TU, attaches to the global module, satisfies both consumption paths at link. |

---

## CLI

Three required, named:

```
python generate.py --name <module> --include <dir> --src <dir>
```

| Flag | Purpose |
|---|---|
| `--name` | Module name. Used in `export module <name>:<part>;`, `module <name>;`, and as the first segment of the impl unit's `#include <<name>/<rel>.hpp>` line. |
| `--include` | Directory holding the canonical inline headers. Scanned recursively. Any path segment named `detail` is skipped. |
| `--src` | Directory to write generated files into. Output mirrors the relative layout under `--include`. |

Missing any flag → argparse usage and exit 2. No defaults; every project's directory shape is different and the script doesn't guess.

### Flat layout

```
my-lib/
├── include/foo/semver.hpp
├── include/foo/buffer.hpp
└── src/
```

```
python generate.py --name foo --include include/foo --src src
```

Produces `src/semver.{decls.hpp,cppm,_impl.cpp}` and `src/buffer.{decls.hpp,cppm,_impl.cpp}`.

The impl units emit `#include <foo/semver.hpp>` etc. The partitions declare `export module foo:semver;` and `export module foo:buffer;`.

### Nested layout

```
lib/foo/
├── include/foo/audio/buffer.hpp
├── include/foo/audio/mixer.hpp
├── include/foo/video/codec.hpp
└── src/
```

```
python generate.py --name foo --include lib/foo/include/foo --src lib/foo/src
```

Output mirrors the layout:

```
lib/foo/src/audio/buffer.{decls.hpp,cppm,_impl.cpp}
lib/foo/src/audio/mixer.{decls.hpp,cppm,_impl.cpp}
lib/foo/src/video/codec.{decls.hpp,cppm,_impl.cpp}
```

Partition names flatten the relative path with underscores: `audio/buffer` → `export module foo:audio_buffer;`. The impl unit's include is `<foo/audio/buffer.hpp>`. The force-emission array name matches the partition: `_emit_audio_buffer_symbols`.

---

## Convention for canonical headers

The parser is a line-scanner with a brace-depth counter, not a full C++ frontend. To keep it small and predictable, canonical headers follow:

1. **One declaration per line at namespace scope.** No `int a, b;`, no two functions on one line.
2. **`inline` is the first keyword** on lines that declare inline functions or variables. Not `static inline`, not `[[nodiscard]] inline`. Attributes go after `inline`.
3. **Namespaces opened with explicit `{` on its own line.** One open/close per line. No `namespace foo { namespace bar { ... } }` on one line.
4. **No `using namespace`** at file scope. Using-declarations (`using std::vector;`) are fine.
5. **Function bodies brace-balanced** and parseable by a depth counter. No preprocessor-conditional braces inside function bodies. No string literals with unmatched braces.

Convention violations produce a line-numbered error from the parser, not silently-wrong output.

---

## Transformation rules

The decls header is the canonical with surgical removals; everything else passes through verbatim:

| In the canonical | In `<rel>.decls.hpp` |
|---|---|
| `inline R foo(args) { body }` | `R foo(args);` |
| `inline T x = v;` | `extern T x;` |
| `inline constexpr …` | **unchanged** — body needed at constant eval |
| `inline consteval …` | **unchanged** |
| `template <…> …` | **unchanged** — body needed at instantiation |
| `struct` / `class` / `enum` (incl. method bodies) | **unchanged** |
| `using NAME = …;` | **unchanged** |
| Non-inline free function decls (ending `);`) | **unchanged** |
| `std::hash<X>` / `std::formatter<X>` specializations | **unchanged** in decls; **not re-exported** by the cppm |
| `#pragma once`, `#include`s, namespace open/close, comments, blank lines | **unchanged** |

The cppm walks the canonical and emits one `using ::ns::name;` per top-level entity inside `export namespace ns { ... }`. Two cases are skipped:

- Entities in any namespace named `detail` (private by convention).
- Specializations of types in `std::` (reached via ADL or explicit lookup, not module export).

The impl unit's force-emission array contains one entry per plain inline function:

```cpp
reinterpret_cast<const void*>(static_cast<R (*)(args)>(&::ns::name))
```

The array is `[[gnu::used]]` on GCC/Clang (defeats `-O2` dead-code elimination); MSVC respects the address-take without it. Areas with no plain inline functions get a placeholder comment instead — the impl unit still exists as a module attachment point.

---

## Wiring it into a build

For xmake, add a task:

```lua
task("codegen")
    on_run(function ()
        os.exec("python scripts/generate.py --name foo --include include/foo --src src")
    end)
    set_menu({
        usage       = "xmake codegen",
        description = "Regenerate per-area decls headers, module partitions, and impl units."
    })
```

Run it via `xmake codegen` whenever a canonical changes, then commit both the canonical edit and the regenerated files. Don't make the build target depend on codegen — consumers and CI should build from a self-contained source tree, never run the generator.

The static-library target globs the generated files:

```lua
target("foo")
    set_kind("static")
    add_files("src/**.cppm", { public = true })
    add_files("src/**.cpp")
    add_includedirs("include", { public = true })
    add_headerfiles("include/(**.hpp)")
target_end()
```

`add_headerfiles("include/(**.hpp)")` ships **only** the canonical headers. The generated `*.decls.hpp` live under `src/` and never get installed — consumers can't accidentally include them.

---

## What's NOT generated, what's skipped

**Not generated** (you hand-write):

- The umbrella `<name>.hpp`
- The primary `<name>.cppm`
- Any `<rel>_native.cpp` for non-inline functions the canonical only declares
- DLL export attributes (`__declspec(dllexport)`, visibility) — the generator targets static-library output. Shared-library builds need attribute decoration on the canonical *and* decls headers consistently, which the script doesn't currently emit.

**Skipped during scanning:**

- Any path segment named `detail` under `--include`. Use `detail/` for hand-written impl plumbing that's part of the public API but not a canonical area — e.g., platform splits the canonical pulls in via `#if defined(_WIN32)`.

---

## Testing a dual-mode library

A dual-mode library should pass **three** test binaries, one per consumption path:

| Binary | What it tests |
|---|---|
| `tests-include` | `#include <name.hpp>` only, **no link** to the static library. Header-only path. |
| `tests-import` | `import name;` only, **links** the static library. Modules-only path. |
| `tests-dual` | Both `#include <name.hpp>` **and** `import name;` in the same TU. The load-bearing test. |

If all three pass on MSVC, GCC, and Clang, the architecture is sound.

Some toolchains need both `#include` and `import` even when the consumer wants pure-import — module GMFs don't always re-export `std::hash` / `std::formatter` specializations or namespace-scope deduction guides. On MSVC VS2022 and GCC 14, the pure-import binary may need to be gated off or only exercise the surface that survives those gaps. VS2026, Clang, and GCC 15+ don't have these limitations.

---

## What changes when you add a function

1. Add one line to `include/<name>/<rel>.hpp` — an `inline` function, an inline variable, a non-inline free decl, whatever.
2. Run `xmake codegen` (or the script directly).
3. The three files at `<src>/<rel>.*` regenerate. Decls picks up the new signature; cppm picks up the new `using`-decl; impl picks up a new force-emission entry.
4. Build and run the three test binaries. Commit canonical + regenerated trio.

No edits to the umbrella header, the primary cppm, the build files, or anything else.

---

## Design properties

- **Single source of truth.** The canonical inline header. Every function body, type definition, and inline variable lives in exactly one place.
- **No DSL, no annotations, no comment markers.** The canonical is regular includable C++23. IDEs, formatters, linters all work without adaptation.
- **The architecture works without the generator.** The four-file pattern is hand-writeable. If the script breaks, edit the four files until it's fixed. The generator is an efficiency tool, not a load-bearing dependency.
- **Replaceable generator.** The convention is conservative C++ that any future tool can parse. A libclang-based replacement can drop into the same pipeline without changing the canonical format or the output layout.
