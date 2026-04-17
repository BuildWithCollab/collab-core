# Test Reorganization Plan — collab::model

> This isn't just a migration. The old test structure was organized per-path (typed, hybrid, dynamic, object), which made coverage gaps invisible. The new structure organizes tests by **capability**, with typed/hybrid/dynamic interleaved so gaps are structurally impossible to ignore.

## Problem

Four test files, each covering one type_def path:

| File | Tests | Path |
|---|---|---|
| test_type_def.cpp | 76 | typed |
| test_hybrid_type_def.cpp | 36 | hybrid |
| test_dynamic_type_def.cpp | 31 | dynamic |
| test_object.cpp | 22 | dynamic (instances) |

This structure hides coverage gaps. An agent adding a test to `test_type_def.cpp` never sees `test_hybrid_type_def.cpp`, so they never think "does hybrid need this too?" Result: hybrid has zero type-level meta tests, `has_field()` coverage is 6/1/1 across paths, edge cases only tested on typed.

## Solution

Capability-oriented files where typed/hybrid/dynamic tests for the **same behavior** sit within a few lines of each other:

```cpp
// ═══ has_field() — finds fields by name ═══
TEST_CASE("typed: has_field() finds field<> members", ...)
TEST_CASE("hybrid: has_field() finds registered members", ...)
TEST_CASE("dynamic: has_field() finds builder fields", ...)

// ═══ has_field() — rejects unknown names ═══
TEST_CASE("typed: has_field() rejects unknown names", ...)
TEST_CASE("hybrid: has_field() rejects unknown names", ...)
TEST_CASE("dynamic: has_field() rejects unknown names", ...)
```

If you're adding a typed test, the hybrid and dynamic versions are three lines below. You'd have to actively look away to not add them too.

## New file structure

### Shared header

**`test_model_types.hpp`** — all test structs, metadata types, and `reflect_on` specializations in one place. Currently these are duplicated with `_d`, `_h`, `_o` suffixes across the 4 old files. The shared header eliminates all duplication.

Contents:
- Metadata types: `endpoint_info`, `help_info`, `tag_info`, `cli_meta`, `render_meta`
- Typed structs (with `field<>` / `meta<>`): `SimpleArgs`, `Dog`, `MixedStruct`, `MultiTagged`, `MetaOnly`, `SingleField`, `CliArgs`
- Hybrid structs (plain C++): `PlainDog`, `PlainPoint`
- Hybrid structs (with `meta<>`): `MetaDog`
- `reflect_on<T>()` specializations for all of the above (inside `#ifndef COLLAB_FIELD_HAS_PFR`)

### Capability files (9 total)

Each file follows the same pattern:
1. `#include` the shared header + catch2
2. `import collab.core;`
3. Sections grouped by **sub-capability**, with typed/hybrid/dynamic interleaved within each section

---

**`test_schema_queries.cpp`** — name(), field_count(), field_names(), stateless

Sections:
- name() returns the struct/type name — typed, hybrid, dynamic
- field_count() counts fields correctly — typed (5 cases: basic, excludes meta, excludes plain, meta-only, multi-tagged), hybrid, dynamic (3 cases: empty, basic, ignores metas)
- field_names() returns correct names — typed (3 cases), hybrid, dynamic (2 cases)
- Stateless — two type_def instances behave identically (typed; add hybrid, dynamic)
- type() access — object returns its backing type_def (dynamic)

---

**`test_has_field.cpp`** — has_field(), has()

Sections:
- Finds fields by name — typed, hybrid, dynamic
- Rejects unknown names — typed, hybrid, dynamic
- Rejects meta member names — typed, hybrid (via MetaDog)
- Rejects plain member names — typed
- Single-field struct — typed (add hybrid, dynamic)
- Meta-only struct — typed (add dynamic with no fields)
- object has() — dynamic

---

**`test_field_query.cpp`** — field(name), field_view properties

Sections:
- field() returns view with correct name — typed, hybrid, dynamic
- field() works for each field in a multi-field struct — typed (add hybrid, dynamic)
- field_view has_default() — dynamic (typed/hybrid fields don't have runtime defaults)
- field_view default_value<V>() — dynamic
- Single-field struct field access — typed (add hybrid, dynamic)

---

**`test_meta_queries.cpp`** — type-level has_meta(), meta(), meta_count(), metas()

Sections:
- has_meta() detects present metas — typed, hybrid (💀 NEW), dynamic
- has_meta() returns false for absent metas — typed, hybrid (💀 NEW), dynamic
- has_meta() on struct with no metas — typed, hybrid, dynamic
- has_meta() on multi-tagged struct — typed (add dynamic)
- meta() returns the metadata value — typed, hybrid (💀 NEW), dynamic
- meta() returns different metadata types — typed (add hybrid, dynamic)
- meta() returns first when multiple of same type — typed, dynamic
- meta_count() — typed (3 cases), hybrid (💀 NEW), dynamic
- metas() returns all of a given type — typed, hybrid (💀 NEW), dynamic
- metas() single-element — typed (add hybrid, dynamic)
- Meta-only struct has zero fields but metas work — typed (add dynamic)

---

**`test_field_level_metas.cpp`** — with<> per-field metadata

Sections:
- Field with single meta — typed, hybrid, dynamic
- Field with multiple metas — typed, hybrid, dynamic
- Field without meta — typed, hybrid, dynamic
- meta_count and metas on fields — dynamic (add typed, hybrid)
- for_each_field detects field-level metas — typed (add hybrid, dynamic)
- for_each_field reads meta values — typed, hybrid (add dynamic)

---

**`test_for_each.cpp`** — for_each(instance), for_each_field(), for_each_meta()

Sections:
- for_each iterates fields with name and value — typed, hybrid, dynamic (object)
- for_each provides typed value access — typed, hybrid, dynamic (object via field_value)
- for_each provides mutable access — typed, hybrid
- for_each skips meta members — typed
- for_each skips plain members — typed
- for_each on empty/meta-only — typed, dynamic (object)
- for_each with const instance — typed, hybrid
- for_each count matches field_count — hybrid (add typed, dynamic)
- for_each_field iterates field descriptors — typed, hybrid, dynamic
- for_each_field skips meta members — typed (add hybrid, dynamic)
- for_each_field on empty — typed, hybrid, dynamic
- for_each_field can query field metas — hybrid, dynamic (add typed)
- for_each_meta iterates meta values — typed (add hybrid via MetaDog)
- for_each_meta provides correct values — typed
- for_each_meta on struct with no metas — typed, hybrid
- for_each_meta with multiple metas — typed
- for_each_meta with instance — typed

---

**`test_set_get.cpp`** — set(), get() callback, get<V>()

Sections:
- set() assigns fields by type — typed (string, int, bool), hybrid, dynamic (object)
- set() with const char* to string field — typed, hybrid, dynamic (object)
- set() overwrites existing values — typed, dynamic (object) (add hybrid)
- set() works on struct with metas — typed (add hybrid)
- get() callback finds field by name — typed (add hybrid)
- get() callback allows mutation — typed (add hybrid)
- get() callback on const instance — typed (add hybrid)
- get<V>() returns value — typed, hybrid, dynamic (object)
- get<V>() round-trips with set() — typed, hybrid, dynamic (object)
- get<V>() on const instance — typed (add hybrid, dynamic)
- Integration tests — typed (CliArgs), hybrid, dynamic (object)

---

**`test_create.cpp`** — create(), defaults, independence

Sections:
- create() returns default-constructed instance — typed, hybrid, dynamic (object)
- create() preserves field defaults — typed, dynamic (object)
- create() result is mutable and works with set — typed (add hybrid, dynamic)
- Default values from type_def — dynamic (object)
- Fields without defaults get default-constructed values — dynamic (object)
- Construction from type_def — dynamic (object)
- Multiple instances from same type_def are independent — dynamic (object) (add typed, hybrid)

---

**`test_throws.cpp`** — all error path throws

Sections:
- set() throws for unknown field — typed, hybrid, dynamic (object)
- set() throws for type mismatch — typed, hybrid, dynamic (object)
- set() does not modify field on type mismatch — typed, hybrid, dynamic (object)
- set() throws for meta member names — typed, hybrid
- get() callback throws for unknown field — typed, hybrid
- get() callback throws for meta member names — typed, hybrid
- get<V>() throws for unknown field — typed, hybrid, dynamic (object)
- get<V>() throws for type mismatch — typed, hybrid, dynamic (object)
- field() throws for unknown name — typed, hybrid, dynamic
- field() throws on empty type_def — dynamic
- meta() throws for absent meta — dynamic
- field_view default_value throws for wrong type — dynamic (shared class, one test sufficient)
- field_view meta() throws for absent meta — dynamic (shared class, one test sufficient)

---

## Files NOT being migrated

These stay as-is:
- **test_field.cpp** — tests the `field<T>` wrapper type itself (default construction, conversion, operator->, with<> extensions, is_field concept, reflected_struct concept)
- **test_meta.cpp** — tests the `meta<T>` wrapper type itself (construction, value access, operator->, is_meta concept, aggregate usage)
- **test_type_schema.cpp** — tests the `type_definition` concept and generic functions constrained by it

## Migration process

1. Create `test_model_types.hpp` with all shared types
2. Create each capability file one at a time, building and running after each
3. Old and new files coexist — test count only goes up
4. Track progress in `docs/test-migration-checklist.md` — subagent verification only
5. When checklist is 100% checked, delete old files one at a time, verifying test count drops by exactly the right amount
6. Delete checklist and this plan doc
7. Add CLAUDE.md rule: agents must maintain coverage for all features across typed/hybrid/dynamic — no capability tested on one path may be left untested on another applicable path

## Gap fills (💀 NEW tests to write during migration)

These are tests that don't exist today but should. They'll be written as new tests in the capability files:

- Hybrid: has_meta(), meta(), meta_count(), metas() (zero coverage today)
- Hybrid: has_field() for meta names, plain members, edge cases (1 test today vs typed's 6)
- Dynamic: stateless (two type_def instances equivalent)
- Dynamic/hybrid: single-field edge cases
- Hybrid: set() overwrites existing values
- Hybrid: get() callback (non-throw happy path)
- Hybrid/dynamic: get<V>() on const instance
- Typed/hybrid: multiple instances are independent
