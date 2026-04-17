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
| test_type_def.cpp | 76 | pending |
| test_hybrid_type_def.cpp | 36 | pending |
| test_dynamic_type_def.cpp | 31 | pending |
| test_object.cpp | 22 | pending |
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

## test_type_def.cpp (76 tests)

### → test_schema_queries.cpp
- [ ] `type_def name() returns the struct type name`
- [ ] `type_def name() works for various types`
- [ ] `type_def field_count() counts only field<> members`
- [ ] `type_def field_count() excludes meta<> members`
- [ ] `type_def field_count() excludes plain members`
- [ ] `type_def field_count() is zero when struct has only metas`
- [ ] `type_def field_count() for multi-tagged struct`
- [ ] `type_def field_names() returns field<> member names only`
- [ ] `type_def field_names() excludes meta and plain members`
- [ ] `type_def field_names() for simple struct`
- [ ] `type_def is stateless — two instances behave identically`

### → test_has_field.cpp
- [ ] `type_def has_field() finds field<> members by name`
- [ ] `type_def has_field() returns false for unknown names`
- [ ] `type_def has_field() returns false for meta member names`
- [ ] `type_def has_field() returns false for plain member names`
- [ ] `type_def has_field() on single-field struct`
- [ ] `type_def has_field() on meta-only struct`

### → test_field_query.cpp
- [ ] `type_def field() returns view with correct name`
- [ ] `type_def field() works for each field`
- [ ] `type_def with single field struct`
- [ ] `type_def default field values are accessible`

### → test_meta_queries.cpp
- [ ] `type_def has_meta() detects present metas`
- [ ] `type_def has_meta() returns false for absent metas`
- [ ] `type_def has_meta() returns false when struct has no metas`
- [ ] `type_def has_meta() works for multi-tagged struct`
- [ ] `type_def meta() returns the metadata value`
- [ ] `type_def meta() returns different metadata types`
- [ ] `type_def meta() returns first when multiple of same type`
- [ ] `type_def meta_count() returns correct count`
- [ ] `type_def meta_count() for multiple metas of same type`
- [ ] `type_def meta_count() is zero for no-meta struct`
- [ ] `type_def metas() returns all metas of a given type`
- [ ] `type_def metas() returns single-element vector for one meta`
- [ ] `type_def with meta-only struct has zero fields`

### → test_field_level_metas.cpp
- [ ] `type_def for_each_field detects field-level metas`
- [ ] `type_def for_each_field reads meta values via meta<M>()`
- [ ] `type_def for_each_field — query has no meta`

### → test_for_each.cpp
- [ ] `type_def for_each iterates field<> members with name and value`
- [ ] `type_def for_each provides access to field values`
- [ ] `type_def for_each provides mutable value access`
- [ ] `type_def for_each skips meta<> members`
- [ ] `type_def for_each skips plain members`
- [ ] `type_def for_each on meta-only struct calls nothing`
- [ ] `type_def for_each with const instance`
- [ ] `type_def for_each_field iterates field descriptors`
- [ ] `type_def for_each_field skips meta members`
- [ ] `type_def for_each_field on meta-only struct yields nothing`
- [ ] `type_def for_each_meta iterates meta values (schema-only)`
- [ ] `type_def for_each_meta provides correct values`
- [ ] `type_def for_each_meta on struct with no metas`
- [ ] `type_def for_each_meta with multiple metas of same type`
- [ ] `type_def for_each_meta with instance reads from that instance`

### → test_set_get.cpp
- [ ] `type_def get() finds field by name`
- [ ] `type_def get() callback allows mutation`
- [ ] `type_def get() on const instance`
- [ ] `type_def set() assigns a string field`
- [ ] `type_def set() assigns an int field`
- [ ] `type_def set() assigns a bool field`
- [ ] `type_def set() works on struct with metas`
- [ ] `type_def set() overwrites existing values`
- [ ] `type_def set() with const char* to string field`
- [ ] `type_def set() round-trips with get()`
- [ ] `type_def get<V>() returns value for matching type`
- [ ] `type_def get<V>() round-trips with set()`
- [ ] `type_def get<V>() on const instance`
- [ ] `type_def CliArgs integration — fields, metas, and extensions`

### → test_create.cpp
- [ ] `type_def create() returns default-constructed instance`
- [ ] `type_def create() preserves field defaults`
- [ ] `type_def create() result is mutable and works with set`

### → test_throws.cpp
- [ ] `type_def get() throws for unknown field name`
- [ ] `type_def get() throws for meta member names`
- [ ] `type_def set() throws for unknown field name`
- [ ] `type_def set() throws for type mismatch`
- [ ] `type_def set() does not modify field on type mismatch`
- [ ] `type_def set() throws for meta member names`
- [ ] `type_def field() throws for unknown name`
- [ ] `type_def get<V>() throws for unknown field`
- [ ] `type_def get<V>() throws for type mismatch`

---

## test_hybrid_type_def.cpp (36 tests)

### → test_schema_queries.cpp
- [ ] `hybrid type_def name()`
- [ ] `hybrid type_def field_count() includes registered fields`
- [ ] `hybrid type_def field_names() includes registered fields`
- [ ] `type_def<T> without hybrid still works`

### → test_has_field.cpp
- [ ] `hybrid type_def has_field()`

### → test_field_query.cpp
- [ ] `hybrid type_def field().name()`

### → test_field_level_metas.cpp
- [ ] `hybrid type_def field with meta`
- [ ] `hybrid type_def field with multiple metas`
- [ ] `hybrid type_def field without meta`

### → test_for_each.cpp
- [ ] `hybrid type_def for_each() iterates registered fields with typed refs`
- [ ] `hybrid type_def for_each() provides real typed values`
- [ ] `hybrid type_def for_each() provides mutable access`
- [ ] `hybrid type_def for_each() with const instance`
- [ ] `hybrid type_def for_each() count matches field_count()`
- [ ] `hybrid type_def for_each_field() iterates registered fields`
- [ ] `hybrid type_def for_each_field() count matches field_count()`
- [ ] `hybrid type_def for_each_field() can query field metas`
- [ ] `hybrid type_def for_each_field() reads meta values`
- [ ] `hybrid type_def for_each_field() on empty hybrid yields nothing`
- [ ] `hybrid type_def for_each_meta() on plain struct yields nothing`
- [ ] `hybrid type_def for_each_meta() on struct with metas`

### → test_set_get.cpp
- [ ] `hybrid type_def set() with matching type`
- [ ] `hybrid type_def set() with const char* to string field`
- [ ] `hybrid type_def get<T>() returns value`
- [ ] `hybrid type_def get<T>() round-trips with set()`
- [ ] `hybrid type_def full integration`

### → test_create.cpp
- [ ] `hybrid type_def create()`

### → test_throws.cpp
- [ ] `hybrid type_def set() throws for unknown field`
- [ ] `hybrid type_def set() throws for type mismatch`
- [ ] `hybrid type_def set() does not modify field on type mismatch`
- [ ] `hybrid type_def set() throws for meta member names`
- [ ] `hybrid type_def get<T>() throws for unknown field`
- [ ] `hybrid type_def get<T>() throws for type mismatch`
- [ ] `hybrid type_def get() callback throws for unknown field`
- [ ] `hybrid type_def get() callback throws for meta member names`
- [ ] `hybrid type_def field() throws for unknown name`

---

## test_dynamic_type_def.cpp (31 tests)

### → test_schema_queries.cpp
- [ ] `dynamic type_def name()`
- [ ] `dynamic type_def name() with different names`
- [ ] `dynamic type_def field_count() with no fields`
- [ ] `dynamic type_def field_count()`
- [ ] `dynamic type_def field_count() ignores type-level metas`
- [ ] `dynamic type_def field_names()`
- [ ] `dynamic type_def field_names() empty`

### → test_has_field.cpp
- [ ] `dynamic type_def has_field()`

### → test_field_query.cpp
- [ ] `dynamic type_def field().name()`
- [ ] `dynamic type_def field().has_default() false`
- [ ] `dynamic type_def field().has_default() true`
- [ ] `dynamic type_def field().default_value<T>()`

### → test_meta_queries.cpp
- [ ] `dynamic type_def has_meta()`
- [ ] `dynamic type_def meta()`
- [ ] `dynamic type_def meta_count()`
- [ ] `dynamic type_def metas()`
- [ ] `dynamic type_def meta() returns first when multiple`
- [ ] `dynamic type_def has_meta() false when no metas`

### → test_field_level_metas.cpp
- [ ] `dynamic type_def field with single meta`
- [ ] `dynamic type_def field with multiple metas`
- [ ] `dynamic type_def field without meta returns false`
- [ ] `dynamic type_def field meta_count and metas`

### → test_for_each.cpp
- [ ] `dynamic type_def for_each_field()`
- [ ] `dynamic type_def for_each_field() empty`
- [ ] `dynamic type_def for_each_field() can query field metas`
- [ ] `dynamic type_def full integration`

### → test_throws.cpp
- [ ] `dynamic type_def field() throws for unknown name`
- [ ] `dynamic type_def field() throws on empty type_def`
- [ ] `dynamic type_def meta() throws for absent meta`
- [ ] `dynamic type_def field_view default_value throws for wrong type`
- [ ] `dynamic type_def field_view meta() throws for absent meta`

---

## test_object.cpp (22 tests)

### → test_schema_queries.cpp
- [ ] `object type() returns the backing type_def`

### → test_has_field.cpp
- [ ] `object has() for existing field`
- [ ] `object has() for missing field`

### → test_for_each.cpp
- [ ] `object for_each() iterates all fields`
- [ ] `object for_each() provides typed access via field_value`
- [ ] `object for_each() on empty type_def`

### → test_set_get.cpp
- [ ] `object set() with matching type`
- [ ] `object set() with const char* to string field`
- [ ] `object set() overwrites existing values`
- [ ] `object get() returns value`
- [ ] `object get() round-trips with set()`

### → test_create.cpp
- [ ] `object construction from type_def`
- [ ] `object constructed via create()`
- [ ] `object has default values from type_def`
- [ ] `object fields without defaults get default-constructed values`
- [ ] `multiple objects from same type_def are independent`
- [ ] `object full integration`

### → test_throws.cpp
- [ ] `object set() throws for unknown field`
- [ ] `object set() throws for type mismatch`
- [ ] `object set() does not modify field on type mismatch`
- [ ] `object get() throws for unknown field`
- [ ] `object get() throws for type mismatch`
