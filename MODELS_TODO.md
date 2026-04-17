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

## ~~💀 Silent error swallowing~~ ✅ FIXED

All 12 silent error paths now throw `std::logic_error` with descriptive messages. 28 Catch2 tests verify exception type + message content. `set()` returns void, `get<V>()` returns V (not optional). Commit `a362814`.

---

## 💀 Test coverage is inconsistent across typed/hybrid/dynamic/object paths

The four paths have wildly uneven test coverage. This needs to be reorganized so every path covers every applicable capability. Current gap analysis:

| Capability | Typed | Hybrid | Dynamic | Object |
|---|---|---|---|---|
| name() | ✅ 2 | ✅ 1 | ✅ 2 | via type() |
| field_count() | ✅ 5 | ✅ 1 | ✅ 3 | via type() |
| field_names() | ✅ 3 | ✅ 1 | ✅ 2 | — |
| has_field() | ✅ 6 | ✅ 1 | ✅ 1 | — |
| field() query | ✅ 3 | ✅ 1 | ✅ 4 | — |
| has_meta() | ✅ 4 | **0** 💀 | ✅ 2 | — |
| meta() | ✅ 3 | **0** 💀 | ✅ 3 | — |
| meta_count() | ✅ 3 | **0** 💀 | ✅ 1 | — |
| metas() | ✅ 2 | **0** 💀 | ✅ 1 | — |
| for_each(instance) | ✅ 7 | ✅ 5 | — | ✅ 3 |
| for_each_field() | ✅ 3 | ✅ 5 | ✅ 3 | — |
| for_each_meta() | ✅ 5 | ✅ 2 | **0** (removed) | — |
| get() callback | ✅ 5 | ✅ 2 | n/a | n/a |
| get<V>() | ✅ 5 | ✅ 4 | n/a | ✅ 4 |
| set() | ✅ 11 | ✅ 7 | n/a | ✅ 7 |
| create() | ✅ 3 | ✅ 1 | n/a | ✅ 2 |
| field-level metas | ✅ 3 | ✅ 4 | ✅ 4 | — |
| defaults | — | — | ✅ 3 | ✅ 2 |
| edge cases | ✅ 3 | **0** 💀 | **0** 💀 | — |

**Key problems:**
- Hybrid has **zero** type-level meta tests (has_meta, meta, meta_count, metas) despite supporting them
- has_field() coverage: typed has 6 cases (unknown names, meta names, plain members, edge cases), hybrid has 1, dynamic has 1
- Edge cases only tested on typed path — single-field struct, meta-only struct, etc. not tested elsewhere
- Test files are organized by path (test_type_def, test_hybrid_type_def, test_dynamic_type_def, test_object) which makes it invisible when one path is missing coverage that another has

**Plan:** Reorganize tests into capability-oriented files (e.g. `test_has_field.cpp`, `test_meta_queries.cpp`, `test_set_get.cpp`, `test_for_each.cpp`) where typed/hybrid/dynamic are adjacent in the same file. This makes gaps structurally visible — if you're looking at `test_has_field.cpp` and hybrid has 1 test while typed has 6, you'd have to be actively trying not to notice.

- [ ] Reorganize test files by capability instead of by path
- [ ] Once done: add a `CLAUDE.md` rule that agents must maintain coverage for all features across each of typed/hybrid/dynamic/object — no capability tested on one path may be left untested on another applicable path

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

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `type_instance` (typed path fully done ✅)
