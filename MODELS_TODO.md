# 🏗️ collab::model — TODO

> Ephemeral — delete items as they're done.

## 🚨 DO NOT MAKE DESIGN DECISIONS ABOUT THE PUBLIC API WITHOUT PURR

If a function is callable by a user of this library, **you do not get to decide how it behaves on error**. You do not get to decide what it returns. You do not get to decide whether it throws or returns nullopt or returns false or returns a default. You bring it to Purr. Every time.

**If you're unsure → ask. If you think it's obvious → ask anyway.**

---

---

## 📋 Still planned

- [ ] **JSON for dynamic path**

  The typed path already has working `to_json(obj)` and `to_json_string(obj)` that serialize any `detail::reflected_struct` to `nlohmann::json` via `type_def<T>.for_each_field()`. The dynamic path (`type_def("Event")` + `type_instance`) has no JSON support yet. This means `type_instance` values can be set/get'd but not serialized. The implementation needs to iterate `type_instance` fields via `for_each_field` (which gives `field_value` wrappers) and produce JSON — but `field_value` is type-erased, so the serializer needs a way to extract the actual value. One approach: register serializers per type at field registration time (similar to how `setter` and `factory` lambdas work). Another: try a fixed set of known types (`int`, `bool`, `std::string`, `double`). Design decision — bring to Purr.
