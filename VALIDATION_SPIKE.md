# def_type — Remaining Gaps & Test Plan

> Updated 2026-04-18 (evening). Reflects state after hybrid nested validation DSL and ODR fix.
>
> **790 test cases, 1972 assertions, all passing.**

---

## ✅ Done — `.parse()` for complex types

Parse body lives in `detail/dynamic_json.hpp` (after `json.hpp`) so it can call `value_from_json`. All complex types work: enums, vectors/maps/optionals of reflected structs, hybrid path. 11 test cases in `test_parse_complex_types.cpp`.

---

## ✅ Done — Hybrid nested validation DSL

Commit `de9b007`. `.field(&T::member, "name", nested_type_def)` overload added to `typed_type_def.hpp`. Nested `type_def` is captured into the reg via `nested_validate_fn` / `nested_valid_fn`. Validates recursively with dotted paths. 7 new tests including 3-level deep nesting and mixed metas+nested schema.

---

## ✅ Done — ODR fix for Color enum

Commit `a478d1b`. Two `enum class Color` definitions with different values across TUs caused magic_enum to pick the wrong instantiation on Linux. Renamed to `ParseColor` in `test_parse_complex_types.cpp`.

---

## Validation test coverage gaps (capability × path)

- [ ] Custom validators — only tested on dynamic, no typed or hybrid tests
- [x] `checked_value()` — tested on all three paths (typed, dynamic, hybrid)
- [x] `operator*` / `operator->` — tested on all three paths
- [ ] "validators don't interfere with set/get/to_json" — only tested on dynamic

---

## `from_json` test gaps

Most from_json scenarios are well-tested on typed + dynamic paths. The audit below reflects actual test file contents, not assumptions.

### ✅ Confirmed tested (typed + dynamic)

- Simple types (string, int, bool, double, float, int64_t, uint64_t)
- Single enum field (string name + int fallback)
- Multiple enum fields in one struct
- Vector of enums
- Map of enums
- Nested structs (`field<Struct>`)
- Vector of structs
- Map of structs
- Optional of struct (present + absent)
- `vector<vector<T>>` (primitives)
- `map<string, vector<T>>` (primitives)
- `optional<vector<T>>` (present + absent)
- `vector<optional<T>>`
- 3-level deep nested structs
- `ankerl::unordered_dense::map` (primitives + structs)
- Sets (std::set, unordered_set) with deduplication
- Round-trip tests for all of the above

### Remaining gaps — nested containers of structs

These combinations work for primitives but have no tests with reflected structs as the inner type:

- [ ] `vector<vector<struct>>` — no test at all
- [ ] `map<string, vector<struct>>` — no test at all
- [ ] Empty containers of structs (empty `vector<struct>`, empty `map<string, struct>`)

### Remaining gaps — hybrid path from_json

Hybrid from_json only covers basic primitives + missing/extra keys. Not tested:

- [ ] Hybrid from_json with enum fields
- [ ] Hybrid from_json with complex collections (vector/map of structs)

---

## Validation test file quality

`test_validation.cpp` was written as a spike. It works and covers a lot, but:

- Naming: still has `"spike:"` prefixes on many test names
- Structure: doesn't follow the clean typed/hybrid/dynamic grouping pattern used by other test files
- Some capabilities only tested on one path when they should be tested on all three

**Plan:** Rename current file to `test_validation_legacy.cpp`, write a clean replacement following the standard pattern, then delete the legacy file once coverage is confirmed equal or better.
