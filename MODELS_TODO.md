# 🏗️ collab::model — TODO

> Ephemeral — delete items as they're done.

## 🚨 DO NOT MAKE DESIGN DECISIONS ABOUT THE PUBLIC API WITHOUT PURR

If a function is callable by a user of this library, **you do not get to decide how it behaves on error**. You do not get to decide what it returns. You do not get to decide whether it throws or returns nullopt or returns false or returns a default. You bring it to Purr. Every time.

**If you're unsure → ask. If you think it's obvious → ask anyway.**

---

## 📋 Open gaps

- [ ] **Hybrid without field<> members — JSON**: Plain structs registered via `type_def<PlainDog>().field(&PlainDog::name, "name")` can't use `from_json<PlainDog>()` because the template requires `reflected_struct` (which needs `field<>` members). Needs a `from_json` / `to_json` overload that takes a `type_def<T>` alongside the struct.

- [x] ~~**`const_cast` in `field_json.cpp`**~~ → codec fields are now `mutable`, zero `const_cast` remaining in src/

---

## 🧪 `field<type_def<>>` — prototype, needs productionizing

DSL for nesting a runtime-defined type inside a struct:

```cpp
auto details_type = type_def("Details")
    .field<std::string>("note")
    .field<int>("priority", 5);

struct Container {
    field<std::string>    name;
    field<type_def<>>     details {details_type};
};
```

**Works today** (prototype in `test_field_type_def.cpp`):
- ✅ `field<type_def<>>` construction from a `type_def`
- ✅ `operator->` for `set()`/`get()` access
- ✅ Defaults from the nested type_def
- ✅ Independent instances
- ✅ `with<>` single + multiple extensions
- ✅ 3-level deep nesting
- ✅ `to_json_string()` via the instance

**Needs doing:**
- [x] ~~Move the `field<type_def<>>` specialization from `test_field_type_def.cpp` into the module~~ → lives in `type_def.cppm` after `type_instance`
- [x] ~~Remove `[prototype]` tag from tests, use proper tags matching other test files~~
- [x] ~~Use shared meta types from `test_model_types.hpp` instead of local `cli_info`/`help_info` redefinitions~~
- [x] ~~Add `struct_info` / PFR fallback so reflection works on structs containing `field<type_def<>>`~~
- [x] ~~Wire up `value_to_json` / `value_from_json` so `to_json(container)` works on structs with `field<type_def<>>` members~~
- [x] ~~Weak `to_json_string` test~~ → now checks actual compact JSON content
