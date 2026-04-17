# 🏗️ collab::model — TODO

> Ephemeral — delete items as they're done.

## 🚨 DO NOT MAKE DESIGN DECISIONS ABOUT THE PUBLIC API WITHOUT PURR

If a function is callable by a user of this library, **you do not get to decide how it behaves on error**. You do not get to decide what it returns. You do not get to decide whether it throws or returns nullopt or returns false or returns a default. You bring it to Purr. Every time.

**If you're unsure → ask. If you think it's obvious → ask anyway.**

---

## 📋 Open gaps

- [ ] **Dynamic-nesting-dynamic**: A dynamic `type_def` with a `field<type_instance>("nested")` won't serialize. `value_to_json<type_instance>` has no specialization — it falls through to the default `nlohmann::json(v)` which doesn't know how to convert a `type_instance`. Fix: add a `type_instance` case to `value_to_json` / `value_from_json` that calls `type_instance::to_json()` / `type_instance::load_json()` recursively.

- [ ] **Hybrid without field<> members — JSON**: Plain structs registered via `type_def<PlainDog>().field(&PlainDog::name, "name")` can't use `from_json<PlainDog>()` because the template requires `reflected_struct` (which needs `field<>` members). Needs a `from_json` / `to_json` overload that takes a `type_def<T>` alongside the struct.

- [ ] **`const_cast` in `field_json.cpp`**: `to_json()` and `load_json()` use `const_cast` on `dynamic_field_def` to lazily init the JSON codec. Works in practice (type_defs are always mutable) but is technically UB if a `type_def` were declared `const`. Consider eagerly initializing the codec or making the codec fields `mutable`.
