# 🏗️ collab::model — TODO

> Gaps verified against test files on 2026-04-16. Ephemeral — delete items as they're done.

---

## 🔥 Broken / Inconsistent

- [ ] **Hybrid `for_each_field(fn)` doesn't iterate hybrid-registered fields** — but `field_count()` and `field_names()` DO include them. Inconsistency landmine.
- [ ] **Hybrid `for_each(instance, fn)` doesn't iterate hybrid-registered fields** — same inconsistency.
- [ ] **Typed `field(name)` returns garbage `thread_local` fallback** for auto-discovered fields. Only works for hybrid-registered fields. 💀
- [ ] **Dynamic `for_each_meta(fn)` doesn't exist** — typed has it, dynamic doesn't.

---

## 🧩 `type_schema` concept — incomplete

> ⚠️ Name not approved by Purr. Needs sign-off.

Currently only checks: `name()`, `field_count()`, `field_names()`, `has_field()`

Missing from concept:
- [ ] `has_meta<M>()`
- [ ] `meta<M>()`
- [ ] `meta_count<M>()`
- [ ] `metas<M>()`
- [ ] `for_each_field(fn)`
- [ ] `for_each_meta(fn)` (needs dynamic impl first)
- [ ] `create()` (different return types across modes — design question)

Also: `dynamic_tag` leaks into public API (`type_def<dynamic_tag>` in static_assert).

---

## 🧪 Missing tests (methods exist, zero coverage)

### Typed `type_def<T>`
- [ ] `has_field(name)`
- [ ] `field(name)` — descriptor by runtime name
- [ ] `get<T>(instance, name)` — optional-returning overload
- [ ] `create()`

### Hybrid `type_def<T>()`
- [ ] `for_each_meta(fn)`

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `object` (typed path fully done ✅)
