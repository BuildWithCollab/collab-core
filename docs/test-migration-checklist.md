# Test Migration Checklist

> Tracks the migration from per-path test files to capability-oriented test files.
> Delete this file when migration is complete.

## Rules

1. **Old and new files coexist.** No old file is deleted until every test it contains is checked off below.
2. **ONLY A SUBAGENT MAY DETERMINE IF A TEST HAS BEEN SUCCESSFULLY MIGRATED.** The main agent may never themselves state that any tests have been migrated. Only reports from subagents may be trusted enough to result in updating this document. Subagents must read both the old test and the new test and confirm the new test covers the same behavior.
3. **Test count must only go up** while old and new coexist. When an old file is deleted, test count drops by exactly the number of tests in that file.
4. **Tests are interleaved by capability**, not grouped by path. Typed/hybrid/dynamic variants of the same behavior sit adjacent to each other.

## Source files being migrated

| Old file | Test count | Status |
|---|---|---|
| test_type_def.cpp | 76 | ✅ migrated |
| test_hybrid_type_def.cpp | 36 | ✅ migrated |
| test_dynamic_type_def.cpp | 31 | ✅ migrated |
| test_type_instance.cpp | 22 | ✅ migrated |
| **Total** | **165** | |

## Destination files

| New file | Capabilities covered |
|---|---|
| test_model_types.hpp | Shared test structs, metadata types, reflect_on specializations |
| test_schema_queries.cpp | name(), field_count(), field_names(), stateless |
| test_has_field.cpp | has_field() |
| test_field_query.cpp | field(name), field_view, has_default(), default_value() |
| test_meta_queries.cpp | has_meta(), meta(), meta_count(), metas() (type-level) |
| test_field_level_metas.cpp | with<> per-field metas |
| test_for_each.cpp | for_each(instance), for_each_field(), for_each_meta() |
| test_set_get.cpp | set(), get() callback, get<V>() |
| test_create.cpp | create(), defaults, independence |
| test_throws.cpp | All error path throws |

---

## test_type_def.cpp (76 tests) — ✅ ALL MIGRATED

### → test_schema_queries.cpp
- [x] `type_def name() returns the struct type name`
- [x] `type_def name() works for various types`
- [x] `type_def field_count() counts only field<> members`
- [x] `type_def field_count() excludes meta<> members`
- [x] `type_def field_count() excludes plain members`
- [x] `type_def field_count() is zero when struct has only metas`
- [x] `type_def field_count() for multi-tagged struct`
- [x] `type_def field_names() returns field<> member names only`
- [x] `type_def field_names() excludes meta and plain members`
- [x] `type_def field_names() for simple struct`
- [x] `type_def is stateless — two instances behave identically`

### → test_has_field.cpp
- [x] `type_def has_field() finds field<> members by name`
- [x] `type_def has_field() returns false for unknown names`
- [x] `type_def has_field() returns false for meta member names`
- [x] `type_def has_field() returns false for plain member names`
- [x] `type_def has_field() on single-field struct`
- [x] `type_def has_field() on meta-only struct`

### → test_field_query.cpp
- [x] `type_def field() returns view with correct name`
- [x] `type_def field() works for each field`
- [x] `type_def with single field struct`
- [x] `type_def default field values are accessible`

### → test_meta_queries.cpp
- [x] `type_def has_meta() detects present metas`
- [x] `type_def has_meta() returns false for absent metas`
- [x] `type_def has_meta() returns false when struct has no metas`
- [x] `type_def has_meta() works for multi-tagged struct`
- [x] `type_def meta() returns the metadata value`
- [x] `type_def meta() returns different metadata types`
- [x] `type_def meta() returns first when multiple of same type`
- [x] `type_def meta_count() returns correct count`
- [x] `type_def meta_count() for multiple metas of same type`
- [x] `type_def meta_count() is zero for no-meta struct`
- [x] `type_def metas() returns all metas of a given type`
- [x] `type_def metas() returns single-element vector for one meta`
- [x] `type_def with meta-only struct has zero fields`

### → test_field_level_metas.cpp
- [x] `type_def for_each_field detects field-level metas`
- [x] `type_def for_each_field reads meta values via meta<M>()`
- [x] `type_def for_each_field — query has no meta`

### → test_for_each.cpp
- [x] `type_def for_each iterates field<> members with name and value`
- [x] `type_def for_each provides access to field values`
- [x] `type_def for_each provides mutable value access`
- [x] `type_def for_each skips meta<> members`
- [x] `type_def for_each skips plain members`
- [x] `type_def for_each on meta-only struct calls nothing`
- [x] `type_def for_each with const instance`
- [x] `type_def for_each_field iterates field descriptors`
- [x] `type_def for_each_field skips meta members`
- [x] `type_def for_each_field on meta-only struct yields nothing`
- [x] `type_def for_each_meta iterates meta values (schema-only)`
- [x] `type_def for_each_meta provides correct values`
- [x] `type_def for_each_meta on struct with no metas`
- [x] `type_def for_each_meta with multiple metas of same type`
- [x] `type_def for_each_meta with instance reads from that instance`

### → test_set_get.cpp
- [x] `type_def get() finds field by name`
- [x] `type_def get() callback allows mutation`
- [x] `type_def get() on const instance`
- [x] `type_def set() assigns a string field`
- [x] `type_def set() assigns an int field`
- [x] `type_def set() assigns a bool field`
- [x] `type_def set() works on struct with metas`
- [x] `type_def set() overwrites existing values`
- [x] `type_def set() with const char* to string field`
- [x] `type_def set() round-trips with get()`
- [x] `type_def get<V>() returns value for matching type`
- [x] `type_def get<V>() round-trips with set()`
- [x] `type_def get<V>() on const instance`
- [x] `type_def CliArgs integration — fields, metas, and extensions`

### → test_create.cpp
- [x] `type_def create() returns default-constructed instance`
- [x] `type_def create() preserves field defaults`
- [x] `type_def create() result is mutable and works with set`

### → test_throws.cpp
- [x] `type_def get() throws for unknown field name`
- [x] `type_def get() throws for meta member names`
- [x] `type_def set() throws for unknown field name`
- [x] `type_def set() throws for type mismatch`
- [x] `type_def set() does not modify field on type mismatch`
- [x] `type_def set() throws for meta member names`
- [x] `type_def field() throws for unknown name`
- [x] `type_def get<V>() throws for unknown field`
- [x] `type_def get<V>() throws for type mismatch`

---

## test_hybrid_type_def.cpp (36 tests) — ✅ ALL MIGRATED

### → test_schema_queries.cpp
- [x] `hybrid type_def name()`
- [x] `hybrid type_def field_count() includes registered fields`
- [x] `hybrid type_def field_names() includes registered fields`
- [x] `type_def<T> without hybrid still works`

### → test_has_field.cpp
- [x] `hybrid type_def has_field()`

### → test_field_query.cpp
- [x] `hybrid type_def field().name()`

### → test_field_level_metas.cpp
- [x] `hybrid type_def field with meta`
- [x] `hybrid type_def field with multiple metas`
- [x] `hybrid type_def field without meta`

### → test_for_each.cpp
- [x] `hybrid type_def for_each() iterates registered fields with typed refs`
- [x] `hybrid type_def for_each() provides real typed values`
- [x] `hybrid type_def for_each() provides mutable access`
- [x] `hybrid type_def for_each() with const instance`
- [x] `hybrid type_def for_each() count matches field_count()`
- [x] `hybrid type_def for_each_field() iterates registered fields`
- [x] `hybrid type_def for_each_field() count matches field_count()`
- [x] `hybrid type_def for_each_field() can query field metas`
- [x] `hybrid type_def for_each_field() reads meta values`
- [x] `hybrid type_def for_each_field() on empty hybrid yields nothing`
- [x] `hybrid type_def for_each_meta() on plain struct yields nothing`
- [x] `hybrid type_def for_each_meta() on struct with metas`

### → test_set_get.cpp
- [x] `hybrid type_def set() with matching type`
- [x] `hybrid type_def set() with const char* to string field`
- [x] `hybrid type_def get<T>() returns value`
- [x] `hybrid type_def get<T>() round-trips with set()`
- [x] `hybrid type_def full integration`

### → test_create.cpp
- [x] `hybrid type_def create()`

### → test_throws.cpp
- [x] `hybrid type_def set() throws for unknown field`
- [x] `hybrid type_def set() throws for type mismatch`
- [x] `hybrid type_def set() does not modify field on type mismatch`
- [x] `hybrid type_def set() throws for meta member names`
- [x] `hybrid type_def get<T>() throws for unknown field`
- [x] `hybrid type_def get<T>() throws for type mismatch`
- [x] `hybrid type_def get() callback throws for unknown field`
- [x] `hybrid type_def get() callback throws for meta member names`
- [x] `hybrid type_def field() throws for unknown name`

---

## test_dynamic_type_def.cpp (31 tests) — ✅ ALL MIGRATED

### → test_schema_queries.cpp
- [x] `dynamic type_def name()`
- [x] `dynamic type_def name() with different names`
- [x] `dynamic type_def field_count() with no fields`
- [x] `dynamic type_def field_count()`
- [x] `dynamic type_def field_count() ignores type-level metas`
- [x] `dynamic type_def field_names()`
- [x] `dynamic type_def field_names() empty`

### → test_has_field.cpp
- [x] `dynamic type_def has_field()`

### → test_field_query.cpp
- [x] `dynamic type_def field().name()`
- [x] `dynamic type_def field().has_default() false`
- [x] `dynamic type_def field().has_default() true`
- [x] `dynamic type_def field().default_value<T>()`

### → test_meta_queries.cpp
- [x] `dynamic type_def has_meta()`
- [x] `dynamic type_def meta()`
- [x] `dynamic type_def meta_count()`
- [x] `dynamic type_def metas()`
- [x] `dynamic type_def meta() returns first when multiple`
- [x] `dynamic type_def has_meta() false when no metas`

### → test_field_level_metas.cpp
- [x] `dynamic type_def field with single meta`
- [x] `dynamic type_def field with multiple metas`
- [x] `dynamic type_def field without meta returns false`
- [x] `dynamic type_def field meta_count and metas`

### → test_for_each.cpp
- [x] `dynamic type_def for_each_field()`
- [x] `dynamic type_def for_each_field() empty`
- [x] `dynamic type_def for_each_field() can query field metas`
- [x] `dynamic type_def full integration`

### → test_throws.cpp
- [x] `dynamic type_def field() throws for unknown name`
- [x] `dynamic type_def field() throws on empty type_def`
- [x] `dynamic type_def meta() throws for absent meta`
- [x] `dynamic type_def field_view default_value throws for wrong type`
- [x] `dynamic type_def field_view meta() throws for absent meta`

---

## test_type_instance.cpp (22 tests) — ✅ ALL MIGRATED

### → test_schema_queries.cpp
- [x] `type_instancetype() returns the backing type_def`

### → test_has_field.cpp
- [x] `type_instancehas() for existing field`
- [x] `type_instancehas() for missing field`

### → test_for_each.cpp
- [x] `type_instancefor_each() iterates all fields`
- [x] `type_instancefor_each() provides typed access via field_value`
- [x] `type_instancefor_each() on empty type_def`

### → test_set_get.cpp
- [x] `type_instanceset() with matching type`
- [x] `type_instanceset() with const char* to string field`
- [x] `type_instanceset() overwrites existing values`
- [x] `type_instanceget() returns value`
- [x] `type_instanceget() round-trips with set()`

### → test_create.cpp
- [x] `type_instanceconstruction from type_def`
- [x] `type_instanceconstructed via create()`
- [x] `type_instancehas default values from type_def`
- [x] `type_instancefields without defaults get default-constructed values`
- [x] `multiple type_instances from same type_def are independent`
- [x] `type_instancefull integration`

### → test_throws.cpp
- [x] `type_instanceset() throws for unknown field`
- [x] `type_instanceset() throws for type mismatch`
- [x] `type_instanceset() does not modify field on type mismatch`
- [x] `type_instanceget() throws for unknown field`
- [x] `type_instanceget() throws for type mismatch`
