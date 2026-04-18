# Validation — Remaining Gaps

## Test coverage gaps (capability × path)

- [ ] Custom validators — only tested on dynamic, no typed or hybrid tests
- [ ] `checked_value()` — not tested on hybrid
- [ ] `operator*` / `operator->` — not tested on hybrid
- [ ] "validators don't interfere with set/get/to_json" — only tested on dynamic

## Implementation gaps in `.parse()`

`.parse()` on typed/hybrid uses `j[key].template get<InnerType>()` instead of `value_from_json`. This works for primitives and nlohmann-native types but does NOT work for:

- [ ] Enums — our serialization uses `magic_enum`, nlohmann doesn't know about that
- [ ] Vectors of reflected structs — e.g. `field<std::vector<Address>>`
- [ ] Maps of reflected structs — e.g. `field<std::map<std::string, Address>>`
- [ ] Optionals of reflected structs — e.g. `field<std::optional<Address>>`

All of these work in `from_json<T>()` because it uses `value_from_json`. `.parse()` bypasses it due to the module partition boundary (`value_from_json` is in `:field_json`, `.parse()` is in `:type_def`).

The hybrid `.parse()` population of registered members also uses raw `.template get<MemT>()`, so nested plain structs, enums, vectors-of-structs etc. silently fail there too.

Root cause: `.parse()` can't call `value_from_json` due to circular module partition dependency.

## DSL changes needed

- [ ] Hybrid nested validation — needs `.field(&T::member, "name", nested_type_def)` overload

```cpp
auto address_type = type_def<PlainAddress>()
    .field(&PlainAddress::street, "street", validators(not_empty{}))
    .field(&PlainAddress::zip, "zip", validators(not_empty{}));

auto person_type = type_def<PlainPerson>()
    .field(&PlainPerson::name, "name", validators(not_empty{}))
    .field(&PlainPerson::address, "address", address_type);  // pass nested schema
```
