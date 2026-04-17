# 🏗️ collab::model — TODO

> Gaps verified against test files on 2026-04-16. Ephemeral — delete items as they're done.

---

## 🔥 API gaps

- [ ] **Dynamic `for_each_meta(fn)` missing** — removed because it leaked `std::any`. Needs reimpl with a wrapper type (like `field_value` wraps field data, a `meta_value` would wrap meta data). Typed path has it, dynamic doesn't.
- [ ] **`field_view` vs `field_value` naming is confusing** — Purr couldn't tell them apart. Needs clearer names or consolidation.
- [ ] **Namespace cleanup** — move internal symbols (`is_field`, `is_meta`, `reflected_struct`, `field_descriptor`, `field_count<T>()`, `field_name<I,T>()`, `dynamic_tag`, `has_pfr_backend`, etc.) into `collab::model::detail`. Keep DSL surface clean: `type_def`, `type_definition`, `field`, `with`, `meta`, `object`, `field_view`, `field_value`, `to_json`, `to_json_string`.
- [ ] **Non-PFR registration** — `reflect_on<T>()` and `field_info<T>(...)` could go into their own sub-namespace or `detail`.

---

## 🧩 `type_definition` concept — needs more methods

Currently checks: `name()`, `field_count()`, `field_names()`, `has_field()`, `field()`, `has_meta<M>()`, `meta<M>()`, `meta_count<M>()`, `metas<M>()`

Still missing:
- [ ] `for_each_field(fn)`
- [ ] `create()` (different return types across modes — design question)

Also: `dynamic_tag` leaks into public API (`type_def<dynamic_tag>` in static_assert).

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `object` (typed path fully done ✅)
