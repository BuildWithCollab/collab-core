# Validation — Remaining Gaps

- [x] ~~parse_options on typed/hybrid — zero tests, could be broken~~ ✅ 16 tests added, works
- [x] ~~Typed parse() with nested field<> structs~~ ✅ fixed, 9 tests
- [x] ~~parse_error data correctness~~ ✅ replaced try/catch with REQUIRE_THROWS_MATCHES
- [ ] Dynamic builder: validators without default value — `.field<std::string>("name", validators(not_empty{}))` not tested, might not compile
- [ ] Validators + metas mixed on typed path — only tested on dynamic and hybrid, no typed struct test
- [ ] CLAUDE.md test coverage rule — several capabilities only tested on one or two paths, not all four
- [ ] Hybrid nested validation — needs DSL change: `.field(&T::member, "name", nested_type_def)` overload

```cpp
// Example: hybrid nested validation
auto address_type = type_def<PlainAddress>()
    .field(&PlainAddress::street, "street", validators(not_empty{}))
    .field(&PlainAddress::zip, "zip", validators(not_empty{}));

auto person_type = type_def<PlainPerson>()
    .field(&PlainPerson::name, "name", validators(not_empty{}))
    .field(&PlainPerson::address, "address", address_type);  // pass nested schema
```
