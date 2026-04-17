# Design: Dynamic type_def + type_instance

> **Status:** approved in chat (2026-04-16)
> **Depends on:** existing `type_def<T>` (fully static, already implemented)

---

## Three modes — one API

```
fully dynamic ◄──────────────────────────────────────► fully static

type_def("Event")        type_def<Existing>()          type_def<Dog>
  .field<T>("name")        .field(&E::age, "age")        // auto-discovers
  .meta<M>({...})          .field(&E::name, "name",      // field<>, meta<>
                               with<M>({...}))            // done
```

---

## Mode 1: Fully static (already implemented)

```cpp
type_def<Dog> t;

// Schema
t.name();                                        // string_view
t.field_count();                                  // size_t
t.field_names();                                  // vector<string>

// Type-level meta
t.has_meta<M>();                                  // bool
t.meta<M>();                                      // M
t.meta_count<M>();                                // size_t
t.metas<M>();                                     // vector<M>
t.for_each_meta(fn);                              // fn(auto& meta_value)
t.for_each_meta(instance, fn);                    // fn(auto& meta_value)

// Field iteration
t.for_each(instance, fn);                         // fn(string_view name, auto& value)
t.for_each_field(fn);                             // fn(auto descriptor)

// Field access by name
t.get(instance, "name", fn);                      // bool; fn(string_view, auto&)
t.set(instance, "name", value);                   // bool

// Create
t.create();                                       // T{}
```

---

## Mode 2: Hybrid — existing struct, register + decorate

```cpp
struct ExistingThing {
    std::string name;
    int         count;
    double      weight;
};

auto thing_t = type_def<ExistingThing>()
    .field(&ExistingThing::name, "name",
        with<help_info>({.summary = "The name"}))
    .field(&ExistingThing::count, "count",
        with<render_meta>({.render = {.style = "bold"}}))
    .field(&ExistingThing::weight, "weight");
```

### Schema

```cpp
thing_t.name();                                   // string_view — "ExistingThing"
thing_t.field_count();                             // size_t
thing_t.field_names();                             // vector<string>
```

### Field queries

```cpp
thing_t.has_field("name");                         // bool
thing_t.field("name").name();                      // string_view
thing_t.field("name").has_default();                // bool
thing_t.field("name").default_value<T>();           // T
thing_t.field("name").has_meta<M>();                // bool
thing_t.field("name").meta<M>();                    // M
thing_t.field("name").metas<M>();                   // vector<M>
```

### Type-level meta

```cpp
thing_t.has_meta<M>();                             // bool
thing_t.meta<M>();                                 // M
thing_t.meta_count<M>();                           // size_t
thing_t.metas<M>();                                // vector<M>
thing_t.for_each_meta(fn);                         // fn(auto& meta_value)
```

### Field iteration (schema-only)

```cpp
thing_t.for_each_field(fn);                        // fn(auto& field_def)
```

### Instance operations

```cpp
thing_t.for_each(instance, fn);                    // fn(string_view name, auto& value)
thing_t.get(instance, "name", fn);                 // bool; fn(string_view, auto&)
thing_t.set(instance, "name", value);              // bool
thing_t.create();                                  // ExistingThing{}
```

---

## Mode 3: Fully dynamic — no struct

```cpp
auto event_t = type_def("Event")
    .meta<endpoint_info>({.path = "/events"})
    .meta<help_info>({.summary = "An event"})
    .field<std::string>("title")
    .field<int>("attendees", 100,
        with<render_meta>({.render = {.style = "bold"}}))
    .field<bool>("verbose", false,
        with<cli_meta>({.cli = {.short_flag = 'v'}}),
        with<render_meta>({.render = {.style = "dim"}}));
```

### Schema

```cpp
event_t.name();                                   // string_view — "Event"
event_t.field_count();                             // size_t
event_t.field_names();                             // vector<string>
```

### Field queries

```cpp
event_t.has_field("title");                        // bool
event_t.field("title").name();                     // string_view
event_t.field("attendees").has_default();           // bool
event_t.field("attendees").default_value<int>();    // 100
event_t.field("verbose").has_meta<cli_meta>();      // bool
event_t.field("verbose").meta<cli_meta>();          // cli_meta
event_t.field("verbose").metas<render_meta>();      // vector<render_meta>
```

### Type-level meta

```cpp
event_t.has_meta<endpoint_info>();                 // bool
event_t.meta<endpoint_info>();                     // endpoint_info
event_t.meta_count<endpoint_info>();               // size_t
event_t.metas<endpoint_info>();                    // vector<endpoint_info>
event_t.for_each_meta(fn);                         // fn(auto& meta_value)
```

### Field iteration (schema-only)

```cpp
event_t.for_each_field(fn);                        // fn(auto& field_def)
```

### Create

```cpp
event_t.create();                                  // type_instance with defaults applied
```

---

## type_instance — instance of a dynamic type_def

```cpp
type_instance party(event_t);

// Set (no template needed — type_def knows the field's type)
party.set("title", std::string("Dog Party"));     // bool — true
party.set("attendees", 50);                        // bool — true
party.set("title", 42);                            // bool — false, type mismatch
party.set("nope", 1);                              // bool — false, no such field

// Get (template needed — C++ needs to know the return type)
party.get<std::string>("title");                   // optional<string>{"Dog Party"}
party.get<int>("attendees");                       // optional<int>{50}
party.get<int>("title");                           // nullopt — wrong type
party.get<std::string>("nope");                    // nullopt — no such field

// Has
party.has("title");                                // bool — true
party.has("nope");                                 // bool — false

// Type access
party.type();                                      // const type_def&
party.type().name();                               // "Event"
party.type().field("attendees").has_default();      // true

// Iteration
party.for_each([](std::string_view name, auto& value) {
    // "title"     → std::string
    // "attendees" → int
    // "verbose"   → bool
});
```

---

## Key design decisions

1. **No `.build()`** — use `const` if you want immutability. C++ already has the mechanism.
2. **`.field()` everywhere** — no distinction between `field<T>` wrappers and plain members from the type_def's perspective. One word, one concept.
3. **`.meta<>()` = type-level, `with<>()` = field-level** — no ambiguity about what metadata attaches to.
4. **Type-level metas go before fields** — mirrors the typed struct convention.
5. **`set()` needs no template** — the type_def already knows the field's type, checks `is_constructible`.
6. **`get<T>()` needs a template** — C++ needs to know the return type.
7. **`.create()` returns T or type_instance** — typed/hybrid returns the struct, dynamic returns a type_instance.
8. **`with<>` reused from the typed path** — same concept, same name, consistent DSL.

---

## Implementation steps

### Step 1: Dynamic type_def — schema only

Build the dynamic `type_def` with construction and schema queries. No instance operations yet.

- `type_def(string_view name)` constructor
- `.field<T>(name)`, `.field<T>(name, default_value)`, `.field<T>(name, default_value, with<>...)`
- `.meta<M>(value)`
- `.name()`, `.field_count()`, `.field_names()`
- `.has_field(name)`, `.field(name)` returning a field descriptor
- Field descriptor: `.name()`, `.has_default()`, `.default_value<T>()`, `.has_meta<M>()`, `.meta<M>()`, `.metas<M>()`
- `.has_meta<M>()`, `.meta<M>()`, `.meta_count<M>()`, `.metas<M>()`, `.for_each_meta(fn)`
- `.for_each_field(fn)`
- Type-erased field storage internally (std::any for defaults and meta values)
- Tests for all of the above

### Step 2: type_instance — dynamic instances

Build `type_instance` backed by a dynamic `type_def`.

- `type_instance(const type_def&)` constructor — copies defaults
- `.set(name, value)` — type-checked, returns bool
- `.get<T>(name)` — returns `optional<T>`
- `.has(name)`
- `.type()` — const ref to backing type_def
- `.for_each(fn)`
- `.create()` on dynamic type_def — returns type_instance with defaults
- Tests for all of the above

### Step 3: Hybrid type_def<T> — register + decorate

Extend `type_def<T>` with manual field registration.

- `.field(&T::member, name)` and `.field(&T::member, name, with<>...)`
- Field queries on hybrid: `.has_field()`, `.field()` with descriptor
- `.for_each()`, `.get()`, `.set()` working through member pointers
- `.create()` on typed/hybrid — returns `T{}`
- Tests for all of the above

### Step 4: Polymorphic base (if needed)

Common interface for typed and dynamic type_def so generic code works with either.

- `void print_schema(const type_def_base&)` style usage
- Evaluate whether this is actually needed or can wait
