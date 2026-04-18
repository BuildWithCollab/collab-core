# Validation — Implementation Status

> Tracking doc. See `validation.md` for the full design spec.

---

## ✅ Done

### Core infrastructure
- [x] `validation_error` struct (path, message, constraint)
- [x] `validation_result` struct (ok(), error_count(), errors(), operator bool, iterable)
- [x] `validator_pack<Vs...>` wrapper type
- [x] `validators()` free function creates packs
- [x] Built-in validators: `not_empty`, `max_length`, `positive` (in `collab::model::validations`)
- [x] `extract_short_validator_name()` — cross-compiler constraint name extraction (MSVC `[]`, GCC `@`, Clang clean)
- [x] `nameof-module-support` package (fork with MSVC module fix)

### valid() / validate() — all 4 paths
- [x] **type_instance** — `obj.valid()`, `obj.validate()`
- [x] **typed** — `t.valid(instance)`, `t.validate(instance)`
- [x] **hybrid** — `dog_t.valid(instance)`, `dog_t.validate(instance)`
- [x] **dynamic** — via type_instance

### Validators on fields — all 4 paths
- [x] **dynamic** — `.field<T>("name", default, validators(...))`
- [x] **typed** — `field<T> name { .validators = validators(...) }`
- [x] **hybrid** — `.field(&T::member, "name", validators(...))`
- [x] Validators + `with<>` metas mixable in any order (dynamic + hybrid tested)

### Nested validation
- [x] `validate()` recurses into nested type_instances
- [x] Dotted paths: `"address.street"`, `"middle.leaf.tag"` (3-level deep)
- [x] `valid()` short-circuits through nesting

### parse_result (dynamic path only)
- [x] `parse_result<T>` struct — value + extra_keys + missing_fields + validation_errors
- [x] `type_def::parse(json)` returns `parse_result<type_instance>`
- [x] `operator*` / `operator->` for value access
- [x] `checked_value()` throws if validation errors exist
- [x] Extra keys and missing fields are informational (don't affect `valid()`)

### Custom validators
- [x] User-defined validators work with zero registration (just implement the contract)
- [x] Constraint names auto-derived via `nameof`

---

## ❌ Not done

### parse_result on typed + hybrid paths
- [ ] `parse<Dog>(json)` → `parse_result<Dog>` (free function, typed path)
- [ ] `dog_t.parse(json)` → `parse_result<PlainDog>` (hybrid path)

### parse_options
- [ ] `parse_options` struct: `reject_extra_keys`, `require_all_fields`, `require_valid`, `strict`
- [ ] `parse(json, {.strict = true})` — throws on any issue
- [ ] `parse(json, {.reject_extra_keys = true})` — throws on unknown keys
- [ ] `parse_error` exception carrying structured data (extra_keys, missing_fields, validation_errors)

### Type mismatch graceful handling
- [ ] `parse()` catches JSON type mismatches (e.g. string where int expected)
- [ ] Mismatched fields get their default value instead of throwing
- [ ] Type mismatches tracked in parse_result (how? TBD)

### Nested validation on typed/hybrid
- [ ] Typed structs with nested `field<Address>` — validate walks into nested structs
- [ ] Hybrid structs with nested registered members
