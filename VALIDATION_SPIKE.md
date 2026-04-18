# Validation — Remaining Gaps

- [ ] Hybrid nested validation — hybrid structs with nested registered members
- [ ] parse_options on typed/hybrid — zero tests, could be broken
- [ ] Typed parse() with nested field<> structs — uses nlohmann .get<>() which won't go through our reflection, likely broken
- [ ] Dynamic builder: validators without default value — `.field<std::string>("name", validators(not_empty{}))` not tested, might not compile
- [ ] Validators + metas mixed on typed path — only tested on dynamic and hybrid, no typed struct test
- [ ] parse_error data correctness — only one test checks structured data on the exception
- [ ] CLAUDE.md test coverage rule — several capabilities only tested on one or two paths, not all four
