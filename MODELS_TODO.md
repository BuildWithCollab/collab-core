# 🏗️ collab::model — TODO

> Ephemeral — delete items as they're done.

## 🚨 DO NOT MAKE DESIGN DECISIONS ABOUT THE PUBLIC API WITHOUT PURR

If a function is callable by a user of this library, **you do not get to decide how it behaves on error**. You do not get to decide what it returns. You do not get to decide whether it throws or returns nullopt or returns false or returns a default. You bring it to Purr. Every time.

**If you're unsure → ask. If you think it's obvious → ask anyway.**

---

## 🧹 Cleanup

- [ ] **MSVC `if constexpr` codegen workaround** — `field_indices_` (filtered index sequence) makes the `else if constexpr` branches in `try_set_field`/`try_get_field` dead code. Consider removing them.
- [ ] **`test_for_each.cpp` local meta types** — defines `help_info_fe` and `cli_meta_fe` locally instead of using shared types from `test_model_types.hpp`. Clean up to use shared types everywhere.
- [ ] **Add `CLAUDE.md` rule** — agents must maintain test coverage across typed/hybrid/dynamic/object for every capability. No capability tested on one path may be left untested on another applicable path.

---

## 📋 Still planned

- [ ] **JSON for dynamic path** — `to_json` / `to_json_string` through dynamic `type_def` + `type_instance` (typed path fully done ✅)
