# Handoff: collab::model System

> **Status:** typed path solid, dynamic path next (2026-04-15)
> **Tests:** 410 assertions, 146 test cases — green on VS 2026 + PFR, VS 2022 no PFR, GCC 15, Clang 22

---

## What this is

A three-mode type definition system in `collab::model`:

```
fully dynamic ◄──────────────────────────────────► fully static

type_def("Event")        type_def<HybridDog>()        type_def<Dog>
  .field<string>(…)        .member(&H::age, …)          // auto-discovers everything
  .meta<E>(…)              // auto-discovers field<>     // field<>, meta<>, done
  .build()                 // + manual plain members

object party(event_t)    Dog rex;                       Dog rex;
  .set<T>(…)                                            rex.name = "Rex";
```

The **right side** (fully static) is done. The **left and middle** are next.

---

## What's implemented ✅

### Everything lives in `collab::model`

One namespace, one `using namespace collab::model;` gets everything:
- `field<T>`, `field<T, with<Exts...>>` — data field wrapper with transparent conversion
- `meta<T>` — type-level metadata wrapper (`operator->`, `is_meta` concept)
- `type_def<T>` — stateless typed schema with auto-discovery
- `with<Exts...>` — field extension pack
- `reflect_on<T>()` — manual registration for non-PFR compilers
- `field_descriptor<T, I>` — schema-only field info
- `reflected_struct` concept
- `to_json()`, `to_json_string()` — JSON serialization through `type_def<T>`

### `type_def<T>` API

```cpp
type_def<Dog> t;

// Schema queries
t.name();                          // "Dog"
t.field_count();                   // 3 (only field<> members)
t.field_names();                   // {"name", "age", "breed"}

// Meta queries
t.has_meta<endpoint_info>();       // true
t.meta<endpoint_info>();           // {.path="/dogs", .method="POST"}
t.meta_count<tag_info>();          // 0
t.metas<tag_info>();               // vector<tag_info>{}

// Instance iteration (field<> only, skips meta<> and plain members)
t.for_each(rex, [](std::string_view name, auto& value) { ... });

// Schema-only iteration (field_descriptor with extensions)
t.for_each_field([](auto descriptor) {
    descriptor.name();
    descriptor.has_extension<cli_meta>();
    descriptor.with().cli.short_flag;
});

// Meta iteration
t.for_each_meta([](auto& meta_value) { ... });
t.for_each_meta(rex, [](auto& meta_value) { ... });

// Field access by runtime name
t.get(rex, "age", [&](std::string_view name, auto& value) { ... });
t.set(rex, "age", 5);  // returns bool — true if found + type matches
```

### `set()` type-checking

Uses `std::is_constructible_v` (not `is_assignable_v`) to gate assignments. This avoids `std::string::operator=(char)` silently accepting ints — `set(args, "name", 42)` correctly returns `false`.

### `meta<T>` as struct members

```cpp
struct Dog {
    meta<endpoint_info> endpoint{{.path = "/dogs", .method = "POST"}};
    meta<help_info>     help{{.summary = "A good boy"}};

    field<std::string>  name;
    field<int>          age;
    field<std::string>  breed;
};
```

Auto-discovery categorizes `meta<>` as metadata and `field<>` as data fields. Multiple metas of the same type are supported.

### `reflect_on<T>()` for non-PFR compilers

```cpp
#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::model::reflect_on<Dog>() {
    return collab::model::field_info<Dog>("endpoint", "help", "name", "age", "breed");
}
#endif
```

Lists ALL members (metas + fields + plain) — the dispatch layer categorizes them.

---

## What was killed 🔥

- `reflection<T>` wrapper class — absorbed into `type_def<T>`
- `bound_field<T, I, Ref>` — replaced by `type_def`'s `(string_view, auto&)` callback style
- `reflect<T>()` / `reflect(instance)` entry points — gone
- `collab::field` namespace — everything migrated to `collab::model`
- `test_field_reflect.cpp` and `test_field_reflect_on.cpp` — coverage exists in `test_type_def.cpp`

---

## What's NOT implemented ❌

### 1. Dynamic `type_def` — fluent builder (NEXT)

```cpp
auto event_t = type_def("Event")
    .field<std::string>("title")
    .field<int>("attendees")
    .meta<endpoint_info>({.path = "/events"})
    .build();
```

Needs type-erased field storage, builder pattern. This is the left side of the spectrum.

### 2. `object` — dynamic instance

```cpp
object party(event_t);
party.set<std::string>("title", "Dog Party");
party.get<std::string>("title");
party.type().name();  // "Event"
```

Needs type-erased value storage. Tightly coupled with dynamic `type_def`.

### 3. Hybrid `type_def<T>` with `.member()` registration

```cpp
auto dog_t = type_def<HybridDog>()
    .member(&HybridDog::age, "age")
    .build();
```

The middle of the spectrum — auto-discovers `field<>` and `meta<>`, plus manually registered plain members.

### 4. Polymorphic base

Both typed and dynamic `type_def` share a common interface so `void print_schema(const type_def_base& t)` works with either.

### 5. JSON for dynamic path

`to_json` already works through `type_def<T>` for typed structs. Dynamic path needs the same via `object`.

---

## Architecture

### Module partitions

```
collab.core.cppm          — umbrella, exports all partitions
├── :field                — field<T>, with<>, reflect_on<T>(), field_info
├── :field_reflect        — PFR/registry backends, dispatch, free functions
├── :field_json           — to_json, to_json_string
├── :meta                 — meta<T>, is_meta
├── :type_def             — type_def<T>
├── :error                — error type
├── :semver               — semantic versioning
├── :log                  — logging facade
└── :term                 — terminal colors
```

### How `type_def<T>` works internally

Stateless. All queries dispatch through `collab::model::detail`:
- `dispatch_field_count<T>()` — total members (PFR or `reflect_on`)
- `member_type<I, T>` — type at index I
- `dispatch_field_name_rt<I, T>()` — runtime-safe name at index I
- `dispatch_get_member<I>(obj)` — reference to member at index I
- `is_field` / `is_meta` concepts — categorize each member

PFR name extraction uses runtime `__PRETTY_FUNCTION__` parsing on Clang and GCC (consteval path had a cross-module bug). MSVC uses the consteval path.

### CI matrix

| Platform | Compiler | PFR | Status |
|----------|----------|-----|--------|
| Windows  | VS 2026  | ✅  | ✅ green |
| Windows  | VS 2022  | ❌  | ✅ green |
| Linux    | GCC 15   | ✅  | ✅ green |
| macOS    | Clang 22 | ✅  | ✅ green |

VS 2022 + PFR = ICE in MSVC 14.44. Stays out of the matrix.

---

## Files that matter

- `lib/collab.core/src/field.cppm` — `field<T>`, `with<>`, `reflect_on`, `is_field`
- `lib/collab.core/src/field_reflect.cppm` — PFR/registry backends, dispatch, free functions
- `lib/collab.core/src/field_json.cppm` — JSON serialization via `type_def<T>`
- `lib/collab.core/src/meta.cppm` — `meta<T>`, `is_meta`
- `lib/collab.core/src/type_def.cppm` — `type_def<T>`
- `lib/collab.core/tests/test_type_def.cpp` — 61 test cases
- `lib/collab.core/tests/test_meta.cpp` — 15 test cases
- `lib/collab.core/tests/test_field.cpp` — field basics, with<>, concepts
- `lib/collab.core/tests/test_field_json.cpp` — JSON serialization tests
- `MODELS_TODO.md` — living checklist

---

## Decisions Purr has made

1. **Namespace is `collab::model`** — `collab::field` is dead
2. **`type_def<T>` construction IS discovery** — no `.build()`, no factory
3. **Multiple metas of same type allowed** — `.meta<M>()` returns first, `.metas<M>()` returns all
4. **`reflection<T>` is dead** — absorbed into `type_def`
5. **snake_case everywhere** — standard library feel
6. **`reflect_on<T>()` stays** — needed for VS 2022 for ~1 more year
7. **`set()` returns `bool`** — matching `get()` semantics
8. **Dynamic `type_def` + `object` are next** — the left side of the spectrum
9. **This is about beautiful DSL ergonomics** — if users have to do weird shit to do shit, fuck no

🏴‍☠️
