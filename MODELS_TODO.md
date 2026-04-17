# 🏗️ collab::model — TODO

> Updated 2026-04-17. Ephemeral — delete items as they're done.

## 🚨 DO NOT MAKE DESIGN DECISIONS ABOUT THE PUBLIC API WITHOUT PURR

If a function is callable by a user of this library, **you do not get to decide how it behaves on error**. You do not get to decide what it returns. You do not get to decide whether it throws or returns nullopt or returns false or returns a default. You bring it to Purr. Every time.

This TODO exists because a previous agent silently decided that type mismatches should return `false`, unknown fields should return `nullopt`, and missing metas should return `M{}`. None of those decisions were discussed. All of them hide bugs. Several of them are actual undefined behavior.

**If you're unsure → ask. If you think it's obvious → ask anyway.**

---

## 🔥 Renames

- [ ] **`object` → `type_instance`** — an instance of a type definition
- [ ] **`field_view` / `field_value` naming confusion** — Purr couldn't tell them apart. Needs clearer names or consolidation.

---

## 💀 Silent error swallowing — previous agent decisions made without consulting Purr

Purr's position: this is a schema, not a container. Programmer errors must surface immediately. `has_field()` and `has_meta()` exist for the query case.

### Unchecked `std::any_cast` dereferences — actual UB

- [ ] **`field_view::default_value<V>()`** — dereferences `any_cast` result without null check. Wrong type = nullptr deref.
- [ ] **`field_view::meta<M>()`** — unchecked deref when found. Returns `M{}` (default-constructed) when not found — error becomes invisible.
- [ ] **`field_view::metas<M>()`** — unchecked deref inside the loop.
- [ ] **`field_value::as<V>()` / `const_field_value::as<V>()`** — unconditional deref. `try_as()` exists as safe path but `as()` is a live footgun.

### Returns wrong data as fallback

- [ ] **`type_def<dynamic_tag>::field("nonexistent")`** — returns a `field_view` pointing at the **first field** when name not found. Code silently operates on the wrong field. Source comment says *"A real implementation might throw"*. 💀

### Default-constructed values masking errors

- [ ] **`type_def<dynamic_tag>::meta<M>()`** — returns `M{}` when meta not found. Valid-looking object with default values. Completely invisible failure.

### Silent `false`/`nullopt` conflating different errors

- [ ] **`object::set()`** — returns `false` for field-not-found, type-mismatch, AND setter-rejection. All three indistinguishable.
- [ ] **`object::get<V>()`** — returns `nullopt` for field-not-found AND type-mismatch. Can't tell which.
- [ ] **`type_def<T>::set()`** — same silent `false` pattern on the typed side.
- [ ] **`type_def<T>::get()` (callback version)** — returns `false`, callback silently never runs.
- [ ] **`type_def<T>::get<V>()` (optional version)** — `nullopt` for two different failure modes.
- [ ] **Dynamic setter lambdas** — return `false` on `any_cast` failure, indistinguishable from field-not-found.

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
