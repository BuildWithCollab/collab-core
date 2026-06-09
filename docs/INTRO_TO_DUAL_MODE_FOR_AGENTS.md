# Intro to Dual-Mode — Read This Before You Touch It (for agents)

You're about to work on, or reason about, a library that supports **both**
`#include <collab.hpp>` and `import collab;` — from the same translation unit, on every
mainstream compiler and OS. This document exists to save you several hours.

Here is the short version, stated plainly so there's no ambiguity:

> **The architecture in this repo is the result of exhaustive, adversarial testing across
> many compilers, many compiler *versions*, many structural variations, and all major OS
> platforms. The "simpler" idea you are about to propose has almost certainly already been
> tried here and found to fail. Header units in particular are not the shortcut you think
> they are.**

This is not a request to trust blindly. It's a request to **read the mechanism first**,
because the constraints are real C++ and compiler-bug constraints, not stylistic choices.
If after reading you still believe you have a genuinely better approach, there's a bar for
that — it's at the bottom of this doc — and it is not "it compiled for me once."

---

## What "dual-mode" means and why all three cases are mandatory

A consumer can reach the library three ways:

1. `#include <collab.hpp>` only
2. `import collab;` only
3. **both, in the same translation unit**

Case 3 is the hard one, and you do **not** get to wave it away with "just pick one." It
happens **transitively, without the author choosing it**: an app writes `import collab;`,
and also `#include`s some *other* library whose header does `#include <collab.hpp>` inside
it. Now one TU has collab arriving both ways. If the architecture isn't coherent, that TU
fails to compile with an error pointing at a header the author never typed. In a fleet of
interdependent libraries, this is not an edge case — it is guaranteed to occur.

So: all three must work, cleanly, with no consumer-visible ceremony.

---

## Read these two, in order, before proposing anything

1. **[`IMPORT_WITH_INCLUDE_IMPORTANT_NOTES.md`](./IMPORT_WITH_INCLUDE_IMPORTANT_NOTES.md)**
   — the clean-cut summary: what crosses both paths for free, what needs the codegen
   machinery, the `[[gnu::used]]` portability note, and the two compiler-version gaps
   (VS2022 / GCC 14) that are **not** about inline functions and get confused with them
   constantly.
2. **[`MODULE_DUAL_MODE.md`](./MODULE_DUAL_MODE.md)** — the full forensics: the exact
   failure modes (`C1001`, `C1117`, `LNK2019`), why each file in the four-file
   architecture exists, the shared-state rules, and the verification methodology
   (three test binaries + a deliberately-broken toggle that must produce a specific
   failure matrix).

The generator that maintains the boilerplate is documented in
[`../CODEGEN.md`](../CODEGEN.md).

---

## The graveyard: "simpler" approaches that have already failed here

Every one of these has been attempted. Each fails for a concrete, reproducible reason —
not because nobody was clever enough. The error codes are real and observed.

### ❌ Header units (`import <collab.hpp>;` / `export import <collab.hpp>;`)

A common reach, and a dead end:

- Re-exporting a header unit lands the header's **inline function bodies in the module's
  exported interface**. On MSVC that detonates downstream `import` consumers with the
  **IFC consumer ICE (`C1001`)** — often surfacing at the closing brace of an unrelated
  class.
- Header-unit support is **fragile and inconsistent across compilers**, with open ICEs as
  of recent MSVC, and immature build-system support in both xmake and CMake.
- It **does not compose** with textual `#include` of the same header in the same TU —
  which is exactly case 3, the case you must support.

### ❌ Just `#include` the full header in the cppm's GMF and `export using` everything

i.e. skip the separate body-stripped "decls" header. This **compiles and looks like a
win** — for structs, enums, templates, `constexpr`. Then someone adds one plain
namespace-scope `inline` function, its body lands in the BMI, and the next `import`
consumer hits the **`C1001` ICE**. The decls header (bodies stripped) exists precisely so
the module interface never carries an inline body. It is not optional.

### ❌ Declare the types directly in the module's purview

i.e. `export struct identifier { ... };` after `export module collab;`, instead of
re-exporting global-module declarations. Now the module's `identifier` is
*named-module-attached* and is a **different entity** from the `#include` path's
global-module `identifier`. A mode-3 TU then fails with **`C1117` ("symbol has already
been defined")**. The whole point of the `using ::collab::name;` re-export is that the
module owns **zero** entities — it only re-names existing global-module ones.

### ❌ Trust the compiler to emit the inline bodies for import consumers

i.e. skip the force-emission array. The impl unit's inline bodies get optimized away (they
are never *used* there), so no out-of-line symbol lands in the archive, and `import`-only
consumers fail at link with **`LNK2019` / undefined reference**. And even with the array,
on **GCC/Clang at `-O2` you must add `[[gnu::used]]`** or the optimizer strips the whole
array and you're back to `LNK2019`. MSVC respects the address-take without it. This is the
single real cross-compiler difference, and it is already handled — do not "simplify" it
away.

### ❌ "Just don't use `inline` anywhere"

Not viable. A header-only library *is* inline functions and inline variables — that's the
mechanism, not an incidental. Remove `inline` and you don't have a header-only library,
you have declarations with nowhere for the bodies to live. The machinery exists exactly so
an inherently inline-centric header can *also* be a module.

### ❌ Ship two unrelated libraries (a header one and a module one)

Breaks case 3 / ODR coherence the moment both reach one TU. The single source of truth and
the global-module attachment are what make import and include interchangeable.

### ❌ "Just don't do it" — drop the header, drop the module, or give up on dual-mode

The most common surrender, and the one that disguises itself as mature scope-cutting:
"ship it header-only and drop the module," "ship it as a module and drop the header,"
"modules aren't ready, don't support both," "this is too fragile, walk away."

It isn't judgment — it's abandoning the requirement, which is the entire reason this
library exists. It almost always arrives right after an agent hits one of the errors above
and mistakes *"I couldn't crack it in the last hour"* for *"it can't be cracked."* The
thing being surrendered is **already solved and verified in this repo** — the `C1001` /
`C1117` that prompted the "give up" advice is the exact failure the four-file architecture
exists to defeat. You are looking at the solution while recommending its abandonment.

The cost is real, too: drop the header and you cut off every header-only / non-module /
static-only consumer; drop the module and you cut off every `import` consumer; and the
transitive case-3 collision still has to resolve regardless of what you "pick." "Pick one"
deletes half the audience to dodge work that is already done.

If you're about to recommend giving up: stop and read `MODULE_DUAL_MODE.md` first. The
working answer is already in the tree.

---

## What's actually verified

- The four-file architecture (decls header / inline header / cppm / impl unit), with the
  `using`-re-export and the force-emission array, builds and passes the **three-binary**
  test (`include`-only, `import`-only, **both**) on **MSVC (incl. VS2026 / 14.50), GCC,
  and Clang**, across OS platforms, for **static libraries**.
- The verification is **adversarial**: a deliberately-broken toggle must flip the dual
  test to the *expected* failure (`C1117`) while leaving the include and import binaries
  passing — proving the passing result is load-bearing, not accidental. See
  `MODULE_DUAL_MODE.md`.
- The two known exceptions to "`import` alone gives you everything" are
  **toolchain-version** issues, not architecture issues: VS2022 doesn't surface
  `std::hash`/`std::formatter` specializations through the GMF; GCC 14 doesn't surface
  namespace-scope deduction guides. VS2026, Clang, and GCC 15+ have neither. Workaround:
  those `import` consumers also `#include <collab.hpp>`. (Again: this is **not** about
  inline functions.)

---

## The bar before you deviate

If you believe you've found a simpler architecture, that's welcome — but the burden of
proof is the **full matrix**, not a single happy compile:

1. All three binaries build and pass: `#include`-only, `import`-only, and **both in one
   TU**.
2. On **MSVC, GCC, and Clang** — not just whichever one is in front of you.
3. The surface must include at least one **plain namespace-scope `inline` function** and
   one **mutable `inline` variable**, because those are the cases everything else passes
   *without* exercising.
4. The adversarial toggle still distinguishes a correct build from a broken one (i.e. you
   can show *why* your version works, by showing a nearby version that fails as expected).

"It compiled on my machine on one compiler" is not evidence — every dead end above also
compiled on at least one compiler. That's *why* they're dead ends: they pass cases 1 and 2
and die on case 3, or on the next compiler, or at link.

There is exactly one legitimate way this gets simpler over time: a compiler **fixes** the
underlying bug (most importantly MSVC's IFC consumer ICE for inline-bodies-in-BMI). If you
have **evidence** that's happened on the toolchains in scope — an actual passing matrix,
not a changelog you're hopeful about — raise it. The canonical-header format is designed to
survive that: a future generator can emit a simpler architecture from the same source. But
that's an evidence-gated change, not a hunch you act on mid-task.

---

## TL;DR

- All three consumption modes are mandatory; case 3 happens transitively whether anyone
  wants it or not.
- The four-file architecture + force-emission + `[[gnu::used]]` guard is the verified
  answer, hardened across many compilers/versions/OSes.
- Header units, "just include in the cppm," "declare in the purview," and "skip
  force-emission" are all already-tried dead ends with known error codes (`C1001`,
  `C1117`, `LNK2019`).
- Read `IMPORT_WITH_INCLUDE_IMPORTANT_NOTES.md` and `MODULE_DUAL_MODE.md` before
  proposing anything.
- Want to simplify it? Bring the full passing matrix or evidence of a fixed compiler bug.
  Not vibes.
