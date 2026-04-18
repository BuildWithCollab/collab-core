# def_type — Remaining Gaps & Test Plan

> Updated 2026-04-18 (evening). **806 test cases, 2019 assertions, all passing.**

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

## ✅ Done — from_json test gaps closed

Commit `c79a86f`. 16 new tests covering all previously uncovered from_json scenarios:

- `vector<vector<Address>>` — typed + dynamic + round-trip
- `map<string, vector<Address>>` — typed + dynamic + round-trip
- Empty containers of structs (empty vector + empty map)
- Hybrid from_json with enum fields + round-trip
- Hybrid from_json with vector/map of structs + round-trips

All from_json paths (typed, dynamic, hybrid) now have comprehensive coverage for all supported type combinations.

---

## Remaining — cosmetic only

### Validation test coverage (capability × path)

- [ ] Custom validators — only tested on dynamic, no typed or hybrid tests
- [ ] "validators don't interfere with set/get/to_json" — only tested on dynamic

### Validation test file quality

`test_validation.cpp` was written as a spike. It works and covers a lot, but:

- Naming: still has `"spike:"` prefixes on many test names
- Structure: doesn't follow the clean typed/hybrid/dynamic grouping pattern used by other test files
- Some capabilities only tested on one path when they should be tested on all three

**Plan:** Rename current file to `test_validation_legacy.cpp`, write a clean replacement following the standard pattern, then delete the legacy file once coverage is confirmed equal or better.
