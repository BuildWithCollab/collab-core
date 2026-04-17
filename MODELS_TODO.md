# 🏗️ collab::model — TODO

> Updated 2026-04-17. Ephemeral — delete items as they're done.

---

## 🔥 Renames

- [ ] **`object` → `type_instance`** — an instance of a type definition
- [ ] **`field_view` / `field_value` naming confusion** — Purr couldn't tell them apart. Needs clearer names or consolidation.

---

## 🔥 API gaps

- [ ] **Dynamic `for_each_meta(fn)` missing** — removed because it leaked `std::any`. Needs reimpl with a wrapper type (like `field_value` wraps field data). Typed path has it, dynamic doesn't.
- [ ] **Namespace cleanup** — move internal symbols into `collab::model::detail`. Public surface: `type_def`, `type_definition`, `field`, `with`, `meta`, `type_instance`, `to_json`, `to_json_string`. Everything else is `detail`.
- [ ] **Non-PFR registration** — `reflect_on<T>()` and `field_info<T>(...)` need a home (`detail` or sub-namespace).
- [ ] **`dynamic_tag`** leaks into public API (`type_def<dynamic_tag>` in static_assert).

---

## 🧩 `type_definition` concept — needs more methods

Currently checks: `name()`, `field_count()`, `field_names()`, `has_field()`, `field()`, `has_meta<M>()`, `meta<M>()`, `meta_count<M>()`, `metas<M>()`

Still missing:
- [ ] `for_each_field(fn)`
- [ ] `create()` (different return types across modes — design question)

---

## 🎨 Naming convention

- [ ] **PascalCase was Purr's original intent** — the previous agent went snake_case without asking. Purr hasn't decided whether to switch. If switched: `TypeDef`, `TypeDefinition`, `Field`, `With`, `Meta`, `TypeInstance`, etc.

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `type_instance` (typed path fully done ✅)
