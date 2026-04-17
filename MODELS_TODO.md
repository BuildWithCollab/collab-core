# 🏗️ collab::model — TODO

> Ephemeral — delete items as they're done.

## 🚨 DO NOT MAKE DESIGN DECISIONS ABOUT THE PUBLIC API WITHOUT PURR

If a function is callable by a user of this library, **you do not get to decide how it behaves on error**. You do not get to decide what it returns. You do not get to decide whether it throws or returns nullopt or returns false or returns a default. You bring it to Purr. Every time.

**If you're unsure → ask. If you think it's obvious → ask anyway.**

---

## 🔥 Renames

- [ ] **`field_def` / `field_value` naming confusion** — Purr couldn't tell them apart. Needs clearer names or consolidation.

---

## 🔥 API gaps

- [ ] **Dynamic `for_each_meta(fn)` missing** — removed because it leaked `std::any`. Needs reimpl with a wrapper type (like `field_value` wraps field data). Typed path has it, dynamic doesn't. Note: typed path also has an *instance* overload (`type_def<Dog>{}.for_each_meta(rex, callback)`) — dynamic equivalent presumably needed too.
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

## 🧹 Cleanup

- [ ] **MSVC `if constexpr` codegen workaround** — `field_indices_` (filtered index sequence) makes the `else if constexpr` branches in `try_set_field`/`try_get_field` dead code. Consider removing them.
- [ ] **`test_for_each.cpp` local meta types** — defines `help_info_fe` and `cli_meta_fe` locally instead of using shared types from `test_model_types.hpp`. Clean up to use shared types everywhere.
- [ ] **Add `CLAUDE.md` rule** — agents must maintain test coverage across typed/hybrid/dynamic/object for every capability. No capability tested on one path may be left untested on another applicable path.

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `type_instance` (typed path fully done ✅)
