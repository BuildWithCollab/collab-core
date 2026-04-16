# 🏗️ collab::model — TODO

> What's left to build. Update as things get done or the design evolves.

---

## ✅ Done

- `meta<T>` — type-level metadata wrapper, `is_meta` concept, helpers
- `type_def<T>` — typed auto-discovery (name, field_count, field_names, has_meta, meta, metas, for_each, for_each_field, for_each_meta, get, set)
- `field<T>::value_type` alias
- `set()` uses `is_constructible_v` (not `is_assignable_v`) to avoid `string::operator=(char)` gotcha
- Killed `reflection<T>`, `bound_field`, `reflect()` — 773 lines deleted 🔥
- Migrated `field_json.cppm` to use `type_def<T>::for_each()` directly
- Namespace migration: `collab::field` → `collab::model` — one namespace for the whole DSL
- Tests passing on VS 2026 + PFR, VS 2022 no PFR, GCC 15, Clang 22

---

## 🔧 TODO

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
- [ ] Wire `to_json` / `to_json_string` through `type_def` for dynamic path too
- [ ] Typed path already done ✅

---

## 🐛 Known issues

- **VS 2022 + PFR:** ICE in MSVC 14.44 on `pfr::detail::fields_count`. VS 2022 works fine without PFR. This combo stays out of the CI matrix.

---

## 💡 Open questions

- Does the dynamic builder need `.build()` or can it be implicit?
- What's the type-erased storage strategy for `object`? (`std::any` vs variant of common types vs something else)
- How much of the polymorphic base do we actually need right now vs later?
- Duplicate test structs (SimpleArgs, WeatherArgs, MixedStruct) exist across test files with different shapes — consolidation opportunity
