# Field<> DSL Design — MSVC 14.50 Notes

> Written April 2026 after ~2 days of design, investigation, and prototyping
> across multiple agent sessions. This documents what we tried, what failed,
> and why the final `Field<T, with<Exts...>>` shape exists. Future agents
> should read this before attempting to change Field's template parameter
> design or "simplify" it back toward class-type NTTPs.

---

## The bug

**MSVC 14.50 (Visual Studio 2026, toolchain 14.50.35717)** has a codegen
defect where module-exported class templates whose specialization mangling
contains **any class-type NTTP** — anywhere in the template argument chain,
including nested inside type parameters — emit duplicate COMDAT sections
for implicit special members (default ctor, dtor, copy/move) when the same
specialization is textually referenced more than once in a single
translation unit.

**Linker symptom:** `LNK1179: invalid or corrupt file: duplicate COMDAT`

**Trigger conditions (ALL must be true):**
1. C++20 module-exported class template
2. At least one class-type NTTP in the specialization's mangled name
3. `T` (the value type) is non-trivially-destructible (e.g. `std::string`)
4. The same specialization is textually written more than once in one TU

**Not affected:**
- Scalar NTTPs (int, bool, char, char packs, enums)
- Type parameters (even if the type itself was produced from a class-type NTTP elsewhere)
- Function template specializations with class-type NTTPs
- The same specialization used across different TUs (normal COMDAT folding)
- Trivially-destructible T (int, bool, etc.)

---

## The original design that triggered it

```cpp
// Original Field — class-type NTTPs: fixed_string for Name, options for Core
template <fixed_string Name, typename T, options Core = {}, auto... Exts>
struct Field { T value{}; /* ... */ };

// This breaks when two structs in one TU share a specialization:
struct Foo { Field<"name", std::string> name; };
struct Bar { Field<"name", std::string> name; };  // 💀 LNK1179
```

---

## What we tried and what failed

### Approaches that DO NOT fix the bug

| Approach | Why it fails |
|---|---|
| **Clean rebuild** (`xmake clean && xmake build -a`) | Not a stale BMI issue. Same error on fresh build. |
| **Explicit `= default`** on all special members | MSVC still emits duplicate COMDATs for defaulted special members. |
| **User-provided special member bodies** (not `= default`, actual `{}` bodies) | Same duplicate COMDAT on the user-provided bodies. |
| **Base class storage** (`field_storage<T>` owns `value`, Field inherits) | Field's own implicit special members still duplicate — the base didn't change which symbols MSVC synthesizes on the derived class. |
| **Type-wrapper approach** (wrap NTTPs in types: `name_t<"city">`, `core_t<options{}>`) | The class-type NTTPs are still in the mangling, just nested one level deeper. `Field_impl<name_t<"city">, ...>` still encodes `fixed_string` in its mangled name. |
| **Alias template** (`template<...> using CField = Field<...>`) | Alias templates get substituted per use. Each `CField<"name", std::string>` expands to the full `Field<...>` textually. Same count, same bug. |
| **Helper template with `::type`** (`FieldDef<...>::type`) | Same as alias template — substituted per use. |
| **Template `core_tag<options{...}>`** (wrap options NTTP in a type template) | The class-type NTTP is still in the mangling chain: `core_tag<options{...}>` encodes the options struct. |

### Approaches that DO work (but weren't chosen)

| Approach | Trade-off |
|---|---|
| **Non-template `using` alias** defined once per specialization | Works because MSVC only counts textual template-ID parse events. An alias is a name lookup, not a template instantiation. But users must discipline themselves to always define aliases and never write `Field<...>` directly more than once per TU. Not enforceable. |
| **Char-pack name** (`name_pack<'c','i','t','y'>` via `name<"city">` alias template) | Eliminates the fixed_string class-type NTTP from Field's mangling. Works. But still needs a solution for `options{}` (which is also a class-type NTTP). Options can be handled via tag structs, but that's ceremony. |
| **Unique Field names per TU** (test workaround) | Each `Field<"unique_name", T>` is a different specialization. Works, but users shouldn't have to think about this. |

### The approach that was chosen

**`Field<T, with<Exts...>>`** — parameterized only by types, zero class-type NTTPs.

- `T` is a type parameter.
- `with<Exts...>` is a type parameter (a struct inheriting from all `Exts...`).
- Field's name comes from `boost::pfr::get_name` at runtime, not a template parameter.
- Core metadata (desc, required, display_name, hidden) are flat `const char*` / `bool` members initialized via designated init.
- Extension metadata uses domain-specific structs passed to `with<>`.

This design **cannot trigger the MSVC bug** because there are no class-type NTTPs anywhere in Field's specialization chain.

---

## Verification matrix

We ran a systematic matrix of experiments to characterize the bug:

| Test | Pattern | Result |
|---|---|---|
| 1 | One spec, one reference | ✅ Pass |
| 2 | One spec, two references (struct + alias) | ❌ LNK1179 |
| 3 | Two different specs, one reference each | ✅ Pass |
| 4 | Multiple distinct Fields in one struct | ✅ Pass |
| A | Two structs, direct `Field<"name", std::string>` | ❌ LNK1179 |
| B | `using` alias once, two structs via alias | ✅ Pass |
| C | Mixed: one via alias, one direct | ❌ LNK1179 |
| D | 50 structs sharing one alias | ✅ Pass (scales) |
| G | Char-pack Name, no class-type NTTPs | ✅ Pass |
| H | Char-pack Name + class-type `options{}` NTTP | ❌ LNK1179 |
| I | Char-pack Name + type-param Core tag | ✅ Pass |
| M | Template `core_tag<options{...}>` | ❌ LNK1179 |
| N | Alias template `name<"...">` → char-pack | ✅ Pass |
| P | `Field<T>` only, PFR get_name, metadata as member | ✅ Pass |

---

## Key insight

**MSVC 14.50's bug counts textual `Field<template-arg-list>` occurrences
at the template-ID parse-and-instantiate step.** Each time the compiler
parses a template-ID with class-type NTTPs, it emits a fresh COMDAT
section for the specialization's implicit special members instead of
reusing a previously emitted one. The duplicate COMDATs in the same
`.obj` file cause `LNK1179`.

Non-template `using` aliases bypass this because they're resolved at
name lookup time — no template instantiation event, no fresh COMDAT.

Alias templates and `::type` helpers don't help because they require
template substitution, which recreates a fresh template-ID internally.

---

## If MSVC fixes the bug

If a future MSVC version fixes the duplicate-COMDAT defect, the current
`Field<T, with<Exts...>>` design remains correct and doesn't need to
change. The design is independently good:

- Simpler template parameters (just types, no NTTPs)
- Field name from PFR (no string duplication)
- Open extension model via `with<>`
- Clean designated-init DSL

There is no reason to revert to class-type NTTPs even if MSVC is fixed.

---

## Commits

- `15f4330` — Original Field<> with fixed_string Name NTTP (works with unique-name workaround)
- `b308ed8` — Char-pack name experiment (Experiment O)
- `5c71d43` — Shape A + F prototypes with ext_pack
- `55c4623` — **Final design: `Field<T, with<Exts...>>`**
