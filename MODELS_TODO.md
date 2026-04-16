# 🏗️ collab::model — TODO

> What's left to build. Update as things get done or the design evolves.

---

## ✅ Done

- `meta<T>` — type-level metadata wrapper, `is_meta` concept, helpers
- `type_def<T>` — typed auto-discovery (name, field_count, field_names, has_meta, meta, metas, for_each, for_each_field, for_each_meta, get)
- Tests for both (passing MSVC + GCC, Clang fix in progress by another agent)

---

## 🔧 TODO

### Namespace migration
- [ ] Move `field<T>`, `with<>`, `reflect_on<T>()`, field_info, field_descriptor from `collab::field` → `collab::model`
- [ ] Kill the `collab::field` namespace entirely
- [ ] Update all tests and imports
- [ ] Duplicate test structs (SimpleArgs, WeatherArgs, MixedStruct) exist across test files with different shapes — consolidation opportunity during migration

### Kill `reflection<T>`
- [ ] Remove `reflection<T>` wrapper class from `field_reflect.cppm`
- [ ] Absorb anything useful into `type_def<T>` internals or free functions
- [ ] Purr explicitly does not want a wrapper class called `reflection<T>`
- [ ] Migrate or remove `test_field_reflect.cpp` and `test_field_reflect_on.cpp` — they test the old `reflect<T>()` API. Anything not already covered by `test_type_def.cpp` should be ported over.

### `set` by runtime name
- [ ] `type_def<T>{}.set(instance, "field_name", value)` — type-checked assignment
- [ ] Companion to the existing `get()`

### Dynamic `type_def` (fluent builder)
- [ ] Non-templated `type_def("Event").field<T>("name").meta<M>({...}).build()`
- [ ] Type-erased field storage
- [ ] Builder pattern returning an immutable schema

### `object` — dynamic instance
- [ ] `object party(event_t)` — instance of a dynamic type_def
- [ ] `.set<T>("field", value)`, `.get<T>("field")`, `.has("field")`
- [ ] `.type()` returns the schema
- [ ] `.for_each(fn)` iteration
- [ ] Type-erased value storage (`std::any` or variant)

### Hybrid `type_def<T>` with `.member()` registration
- [ ] `type_def<T>().member(&T::plain_member, "name").build()`
- [ ] Registers plain (non-field<>) members alongside auto-discovered field<>/meta<>
- [ ] Member pointers give read + write for free

### Polymorphic base
- [ ] Typed `type_def<T>` and dynamic `type_def` share a common interface
- [ ] `void print_schema(const type_def_base& t)` works with either
- [ ] Requires type-erased runtime storage

### JSON through type_def
- [ ] Wire `to_json` / `to_json_string` through `type_def` instead of old `reflect<T>()` API
- [ ] Support both typed and dynamic paths

---

## 🐛 Known issues

- **Clang CI:** 1 test failure on `CliArgs` (meta + field with extensions combo). Another agent is on this.

---

## 💡 Open questions

- Does the dynamic builder need `.build()` or can it be implicit?
- What's the type-erased storage strategy for `object`? (`std::any` vs variant of common types vs something else)
- How much of the polymorphic base do we actually need right now vs later?
- ~~Should `set` return a bool (like `get`) or throw/error on missing field?~~ → `bool`, matching `get` semantics. Decided.
