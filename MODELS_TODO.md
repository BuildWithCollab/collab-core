# 🏗️ collab::model — TODO

> Gaps verified against test files on 2026-04-16. Ephemeral — delete items as they're done.

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
- [ ] `create()` (different return types across modes — design question)

Also: `dynamic_tag` leaks into public API (`type_def<dynamic_tag>` in static_assert).

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `object` (typed path fully done ✅)
