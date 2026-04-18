# Validation Spike

> Tracking doc for tonight's work. Spike first, then go wide.

---

## Potential Post-Spike Plan (Steps 1-5)

These were our initial ideas — may change after the spike teaches us things.

1. **Validator contract + `validators()` helper** — built-in validators under `collab::model::validators` namespace, `validators()` helper stays in `collab::model`
2. **Attach validators to fields** — dynamic builder, hybrid builder, typed `field<T>` `.validators` member
3. **`validation_result` + `validation_error`** — stateless result types
4. **`valid()` / `validate()` across all 4 paths** — typed, hybrid, dynamic, type_instance
5. **`parse_result<T>` + `parse_options`** — wraps `from_json`, gives object + report

---

## Spike: Dynamic Path Only

Goal: prove the full validation pipe end-to-end before going wide.

- [ ] **Test 1** — single validator on dynamic field, `valid()` returns false
- [ ] **Test 2** — `validate()` returns `validation_result` with error details (path, message)
- [ ] **Test 3** — multiple validators on one field, both can fail
- [ ] **Test 4** — mix of validated and unvalidated fields
- [ ] **Test 5** — fixing values makes `valid()` become true, `validate().ok()`
- [ ] **Test 6** — validators don't interfere with set/get/to_json/schema queries

### What the spike builds (just enough to pass)

- `collab::model::validators` namespace — `not_empty{}`, `max_length{N}`, `positive{}`
- `validators()` helper in `collab::model`
- `dynamic_field_def` gets type-erased validator storage
- `validation_error` and `validation_result` structs
- `type_instance::valid()` and `type_instance::validate()`
- Dynamic `.field<V>()` builder gains overload accepting `validators(...)` alongside `with<>()`

### What the spike does NOT touch

- `field<T>` struct (typed path)
- Hybrid path builder
- `parse_result<T>` / `from_json` changes
