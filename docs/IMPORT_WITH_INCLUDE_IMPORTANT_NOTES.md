# Import + Include: The Clean-Cut Notes

What actually crosses the `#include <collab.hpp>` / `import collab;` boundary, what
needs the codegen machinery to do so, and the two compiler-specific gaps that are
**not** about inline functions and routinely get confused with them.

This is the short, sharp version. For the full mechanism and failure-mode forensics,
see [`MODULE_DUAL_MODE.md`](./MODULE_DUAL_MODE.md); for the generator, see
[`../CODEGEN.md`](../CODEGEN.md).

---

## The one distinction that matters

A consumer can reach collab three ways, and all three must work — including the third,
which often happens *transitively* without the author choosing it (an app does
`import collab;` while also `#include`-ing some other lib whose header includes
`collab.hpp`):

1. `#include <collab.hpp>` only
2. `import collab;` only
3. both, in the same translation unit

For **everything** in your public surface, the architecture guarantees the imported
entity and the included entity are the **same entity** (same global-module attachment,
re-exported by `using`-declaration, never redeclared in the module's purview). That's
what stops mode 3 from blowing up with C1117. It applies to *all* kinds of declarations
equally.

The split below is **not** about that coherence — it's about one extra thing: does the
declaration also need a **runtime body delivered as a link symbol** for an
`import`-only consumer?

| Kind of thing | Crosses both paths cleanly? | Needs the machinery? |
|---|:---:|:---:|
| `struct` / `class` / `enum` / `union` | ✅ | — |
| type aliases (`using X = Y;`) | ✅ | — |
| templates (function / class / variable) | ✅ | — |
| `constexpr` / `consteval` functions | ✅ | — |
| `constexpr` / compile-time variables | ✅ | — |
| in-class member function bodies | ✅ | — |
| **plain namespace-scope `inline` functions** | ✅ | **yes** |
| **out-of-class `inline` member functions** | ✅ | **yes** |
| **mutable `inline` variables** (shared state) | ✅ | **yes** |

Every row works across **all** include/import combinations on every modern compiler.
The right-hand column is only saying *what it takes* to make the bottom three work — the
codegen machinery — not that they're fragile or unsupported.

---

## Why the bottom three need the machinery

The free rows are self-contained: a type carries its full definition through the BMI; a
template / `constexpr` / `consteval` body is *supposed* to stay reachable, so it rides
through and the consumer instantiates or evaluates it locally. No runtime symbol, no
archive.

A **plain inline function body** is different. It has to satisfy two constraints that
pull in opposite directions:

1. **It must stay out of the module's BMI.** An inline function body baked into the BMI
   crashes downstream `import` consumers on MSVC — the IFC consumer ICE (`C1001`). So the
   module interface can only carry a body-free *declaration*.
2. **An `import`-only consumer still has to call it.** With only a declaration in the
   BMI, that call becomes an unresolved external symbol. Something has to provide a real,
   out-of-line definition at link time, or you get `LNK2019` / undefined reference.

The machinery resolves both:

- **decls header** — the canonical with plain `inline` bodies stripped to declarations.
  This is what the module interface includes (in its global module fragment), so the BMI
  stays body-free. (Constraint 1.)
- **cppm** — `using ::collab::name;` re-exports those global-module declarations. Nothing
  is declared in the module's own purview, so `import` and `#include` land on the same
  entity. (Coherence — the thing that's true for *every* row.)
- **impl unit** — `#include`s the real inline header and address-takes each function in a
  force-emission array, forcing the compiler to emit an out-of-line COMDAT copy into the
  compiled archive. (Constraint 2.)

`#include` consumers never touch any of this — they get the inline body as text and inline
it normally. The machinery exists purely so the **`import` path** can offer the same
functions.

Mutable `inline` variables (shared state, e.g. a sink registry) are the same story:
declared `extern` in the decls header, defined once, with the namespace-scope `inline`
variable COMDAT-folded to a single instance across every TU that sees it.

---

## ⚠️ Force-emission portability: `[[gnu::used]]` on GCC/Clang

The force-emission array is one place where compilers genuinely differ, and it's a silent
footgun:

- **GCC and Clang** dead-code-strip the array at `-O2`+ even though `[[maybe_unused]]` is
  present — the variable is never read, so the optimizer drops it, which drops the
  address-take ODR-uses, which drops the symbol emission. The result: **`import`-only
  consumers fail to link** (`undefined reference`).
- **MSVC** respects an address-taken, externally-linkable variable without any extra
  annotation.

The fix is a single guarded attribute on the array:

```cpp
namespace {
#if defined(__GNUC__) || defined(__clang__)
[[gnu::used]]
#endif
[[maybe_unused]] const void* const _emit_<area>_symbols[] = { /* ... */ };
}
```

`[[gnu::used]]` tells the optimizer the variable is observed externally and must be
emitted, which forces the address-takes to materialize as real symbols. This is the
**entire** cross-compiler accommodation for the inline machinery. With it, plain inline
functions work identically on MSVC, GCC, and Clang.

(For `inline` *member* functions, the force-emission uses **pointer-to-member**, not
`reinterpret_cast` to `void*` — PMF layout differs and casting it is UB. See
`MODULE_DUAL_MODE.md`.)

---

## The discriminator the codegen keys on

It's basically **plain `inline`** — with one carve-out:

> `constexpr`, `consteval`, and `template` **trump** `inline`. Their bodies have to stay
> reachable anyway (constant evaluation / instantiation), so they ride through the free
> path even when they're written `inline`.

So the generator strips bodies only from:

- `inline R foo(args) { ... }` → `R foo(args);`
- `inline T x = v;` → `extern T x;`

…and leaves anything carrying `constexpr` / `consteval` / `template` **untouched**. That
single rule is the whole free-vs-machinery split.

---

## ⚠️ The two compiler gaps that are NOT about inline functions

These get conflated with the inline story constantly. They are a **different category**:
not about delivering inline *bodies*, but about certain declarations *surfacing* through
the module's global module fragment to an `import`-only consumer.

| Toolchain | What doesn't surface through pure `import` | Symptom |
|---|---|---|
| **MSVC VS2022** | foreign-template specializations — `std::hash` / `std::formatter` (and `fmt::formatter`) for our types | `std::format("{}", fs)` or `std::hash<fixed_string>{}(fs)` fails to compile |
| **GCC 14** | namespace-scope **deduction guides** | CTAD like `collab::fixed_string s = "hi"` fails to compile |
| **VS2026, Clang, GCC 15+** | — (neither limitation) | works on pure `import` |

**Workaround** on the two affected toolchains: an `import`-only consumer adds
`#include <collab.hpp>` alongside `import collab;` (order doesn't matter). The include
path surfaces what the GMF didn't.

**Key point:** plain inline functions, members, and variables are **not** affected by
this. They go through force-emission, which works on every modern compiler. The flaky
surface is *specializations of foreign templates* and *deduction guides* — and only on
those two older toolchains.

---

## Two jobs, kept separate (the thing that's easy to muddle)

If you remember nothing else, remember that the architecture does **two independent
jobs**, and conflating them is where confusion comes from:

- **Job 1 — same-entity coherence (compile time).** The cppm re-exports global-module
  declarations instead of declaring its own, so `import` and `#include` resolve to one
  entity → no `C1117` in a mixed TU. Applies to **everything** in the table. No archive
  involved.
- **Job 2 — inline-body delivery (link time).** decls-strip dodges the ICE; force-emission
  puts the body in the archive so an `import`-only consumer can link it. Applies **only**
  to plain inline functions / members / mutable inline variables.

Job 1 is about *which entity you're naming*. Job 2 is about *where a body's symbol comes
from*. They're orthogonal.

---

## Scope / verified on

- Verified on **MSVC 14.50 (VS2026)**, **GCC**, and **Clang**, building **static
  libraries**. The `[[gnu::used]]` guard is the only observed cross-compiler difference
  for the inline machinery.
- Shared libraries (DLL / `.so`) are **out of scope** — the architecture is not verified
  there, and collab libraries ship static / header-only, compiled into the terminal
  artifact. (Cross-DLL shared state has its own constraints that simply don't apply when
  nothing is distributed as a standalone dynamic library.)
- The VS2022 / GCC 14 surfacing gaps above are the known exceptions to "`import` alone
  gives you everything," and they are toolchain-version issues, not architecture issues.
