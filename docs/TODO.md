# TODO

## test_throws.cpp should not exist

"throw" is not a capability. Error paths belong with the capability they test:
- `set() throws for unknown field` → `test_set_get.cpp`
- `set() throws for type mismatch` → `test_set_get.cpp`
- `set() does not modify on mismatch` → `test_set_get.cpp`
- `set() throws for meta/plain member names` → `test_set_get.cpp`
- `get() callback throws` → `test_set_get.cpp`
- `get<V>() throws` → `test_set_get.cpp`
- `field() throws for unknown/meta/plain name` → `test_field_query.cpp`
- `field() throws on empty type_def` → `test_field_query.cpp`
- `meta() throws for absent meta` → `test_meta_queries.cpp`
- `field_view.default_value() throws` → `test_field_query.cpp`
- `field_view.meta() throws for absent` → `test_field_level_metas.cpp`
- `set()/get() throws for empty string` → `test_set_get.cpp`

Dissolve `test_throws.cpp` and move each test to its capability file.

## try/catch in throw tests — use REQUIRE_THROWS_AS

53 throw tests use hand-rolled try/catch/FAIL/string-inspection. Replace with `REQUIRE_THROWS_AS(expr, std::logic_error)`. The error message inspection is brittle and tests implementation, not behavior.

Also fixes an MSVC release-mode bug where `e.what()` inside a catch block segfaults due to exception object corruption (already hit and worked around for the plain-member tests).

## Old test files still present

The 4 old per-path test files are still in the tree alongside the new capability files:
- `test_type_def.cpp` — all tests migrated
- `test_hybrid_type_def.cpp` — all tests migrated
- `test_dynamic_type_def.cpp` — all tests migrated
- `test_object.cpp` — all tests migrated

Delete them one at a time, verifying test count drops by exactly the number in each file.

## MSVC if constexpr codegen — filtered index sequence workaround

MSVC 2026 release mode miscompiles fold expressions over index sequences containing non-field member indices. Even with `if constexpr (is_field<member_t>)` guards, the false branch corrupts the stack. Workaround: `field_indices_` (compile-time filtered index sequence) ensures set/get fold expressions only see field<> member indices. The `else if constexpr` branches in `try_set_field`/`try_get_field` are now dead code — consider removing them.

## test_for_each.cpp has local meta types

`test_for_each.cpp` defines `help_info_fe` and `cli_meta_fe` locally instead of using the shared types from `test_model_types.hpp`. Some tests use the local types, some use shared. Clean up to use shared types everywhere.
