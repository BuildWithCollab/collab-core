# 🏗️ collab::model — TODO

> Gaps verified against test files on 2026-04-16. Ephemeral — delete items as they're done.

---

## 🔥 Broken / Inconsistent

- [x] ~~**Hybrid `for_each_field(fn)` doesn't iterate hybrid-registered fields**~~ — fixed, both `field_descriptor` and `field_view` expose `.name()` / `.has_meta<M>()`
- [ ] **Hybrid `for_each(instance, fn)` doesn't iterate hybrid-registered fields** — needs return-type-changing builder so member types survive in template params (no type erasure). Real typed references, no `std::any`.
- [ ] **`object::for_each` leaks `std::any` into the public API** — never approved. Needs redesign so users never see `std::any`.
- [ ] **Rename `field_view` → `field_view`** — it's used across all three modes, nothing "dynamic" about it.

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
- [ ] `for_each_meta(fn)`
- [ ] `create()` (different return types across modes — design question)

Also: `dynamic_tag` leaks into public API (`type_def<dynamic_tag>` in static_assert).

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `object` (typed path fully done ✅)
