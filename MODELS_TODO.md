# 🏗️ collab::model — TODO

> Gaps verified against test files on 2026-04-16. Ephemeral — delete items as they're done.

---

## 🔥 Broken / Inconsistent

- [ ] **Hybrid `for_each_field(fn)` doesn't iterate hybrid-registered fields** — but `field_count()` and `field_names()` DO include them. Inconsistency. ⚠️ Design challenge: typed `for_each_field` passes `field_descriptor<T,I>`, hybrid needs `dynamic_field_view` — mixing both in one generic callback causes compile errors. Needs design discussion.
- [ ] **Hybrid `for_each(instance, fn)` doesn't iterate hybrid-registered fields** — same inconsistency. Also has a type-erasure design challenge: hybrid fields are registered via member pointers but type-erased, so the generic callback can't receive typed references.

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
- [ ] `for_each_meta(fn)` (dynamic impl done ✅, but callback types differ: typed gives `auto&`, dynamic gives `const std::any&`)
- [ ] `create()` (different return types across modes — design question)

Also: `dynamic_tag` leaks into public API (`type_def<dynamic_tag>` in static_assert).

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `object` (typed path fully done ✅)
