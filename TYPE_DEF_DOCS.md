# type_def

**A C++ type definition framework where structs describe themselves.**

Define a type once — with fields, metadata, defaults, and constraints — and get reflection, serialization, validation, and schema queries for free. No macros. No code generation. Just C++23.

## The Problem

C++ gives you structs. Structs are great for holding data, but they don't *know* anything about themselves. Want to iterate over fields? Write it by hand. Want to serialize to JSON? Write a custom function. Want to attach metadata like "this field has a CLI short flag"? There's no standard way.

Every framework solves this differently — Unreal has `UPROPERTY()` macros, Qt has `moc`, Protocol Buffers have `.proto` files and code generation. They all work, but they all force you to leave normal C++ behind.

`type_def` takes a different approach: **your types stay as regular C++ structs**, and the framework works alongside them.

## Quick Start

### The Typed Path — Let Your Struct Be the Schema

```cpp
#include <type_def.h>
using namespace collab::model;

struct Dog {
    meta<endpoint_info> endpoint{{.path = "/dogs", .method = "POST"}};
    meta<help_info>     help{{.summary = "A good boy"}};

    field<std::string>  name;
    field<int>          age;
    field<std::string>  breed;
};

// That's it. The type definition IS the struct.
type_def<Dog> t;

t.name();          // "Dog"
t.field_count();   // 3
t.field_names();   // ["name", "age", "breed"]
t.has_meta<endpoint_info>(); // true
```

`field<T>` is a transparent wrapper — it *is* the value. Implicit conversion, `operator->`, aggregate initialization. Your struct stays an aggregate. Your code reads like normal C++:

```cpp
Dog rex;
rex.name = "Rex";           // just works — implicit conversion
rex.age = 3;
std::string name_copy = rex.name;   // just works — implicit conversion back
rex.name->size();            // operator-> passes through to std::string
```

But now `type_def<Dog>` can iterate fields, query metadata, set/get by name — all with zero overhead for the typed path:

```cpp
// Create an instance through the type_def
Dog rex = t.create();

// Set fields by name — the type_def knows where they live
t.set(rex, "name", std::string("Rex"));
t.set(rex, "age", 3);
t.set(rex, "breed", std::string("Husky"));

// Get fields back by name
t.get<std::string>(rex, "name");   // "Rex"
t.get<int>(rex, "age");            // 3

// Query the schema
t.has_field("name");               // true
t.has_field("nope");               // false

// Access type-level metadata
auto endpoint = t.meta<endpoint_info>();
// endpoint.path == "/dogs", endpoint.method == "POST"

// Iterate all fields with their values
t.for_each_field(rex, [](std::string_view name, auto& value) {
    // name: "name", "age", "breed"
    // value: typed reference — std::string&, int&, std::string&
});
```

### The Dynamic Path — No Struct Required

Define a type entirely at runtime:

```cpp
auto event_t = type_def("Event")
    .meta<endpoint_info>({.path = "/events"})
    .meta<help_info>({.summary = "An event"})
    .field<std::string>("title")
    .field<int>("attendees", 100)     // 100 is the default
    .field<bool>("verbose", false,
        with<cli_meta>({.cli = {.short_flag = 'v'}}));
```

Create instances and work with them:

```cpp
auto party = event_t.create();

// Defaults are populated automatically
party.get<int>("attendees");       // 100
party.get<bool>("verbose");        // false
party.get<std::string>("title");   // "" (default-constructed)

// Set fields
party.set("title", std::string("Dog Party"));
party.set("attendees", 50);
party.set("verbose", true);

// Get fields back
party.get<std::string>("title");   // "Dog Party"
party.get<int>("attendees");       // 50

// Check if fields exist
party.has("title");                // true
party.has("nope");                 // false

// Access the backing type_def through the instance
party.type().name();               // "Event"
party.type().field_count();        // 3
party.type().has_meta<endpoint_info>(); // true
party.type().field("attendees").has_default();       // true
party.type().field("attendees").default_value<int>(); // 100

// Iterate all fields with their values
party.for_each_field([](std::string_view name, field_value value) {
    if (name == "title")
        auto& title = value.as<std::string>();  // typed access
    if (name == "attendees")
        auto count = value.as<int>();
});

// Iterate the schema (no instance needed)
event_t.for_each_field([](field_def field_descriptor) {
    field_descriptor.name();          // "title", "attendees", "verbose"
    field_descriptor.has_default();   // false, true, true
    field_descriptor.has_meta<cli_meta>(); // false, false, true
});
```

### The Hybrid Path — Plain Structs, Registered Fields

Already have a struct? Don't want to change it? Register fields via member pointers:

```cpp
struct PlainDog {
    std::string name;       // plain std::string — not field<>
    int         age = 0;    // plain int
    std::string breed;      // plain std::string
};
```

This is a completely normal C++ struct. You can use it normally:

```cpp
PlainDog rex;
rex.name = "Rex";
rex.age = 3;
```

Now register it with `type_def` to get the full reflection API — without changing the struct at all:

```cpp
auto t = type_def<PlainDog>()
    .field(&PlainDog::name, "name",
        with<help_info>({.summary = "Dog's name"}))
    .field(&PlainDog::age, "age",
        with<help_info>({.summary = "Age in years"}),
        with<render_meta>({.render = {.style = "bold"}}))
    .field(&PlainDog::breed, "breed");
```

Now you get the exact same API as typed and dynamic:

```cpp
// Schema queries — same as typed and dynamic
t.name();          // "PlainDog"
t.field_count();   // 3
t.field_names();   // ["name", "age", "breed"]
t.has_field("name");  // true
t.has_field("nope");  // false

// Create an instance
PlainDog buddy = t.create();

// Set fields by name — these are plain std::string and int, not field<>
t.set(buddy, "name", std::string("Buddy"));
t.set(buddy, "age", 5);
t.set(buddy, "breed", std::string("Lab"));

// The struct is just a normal struct — you set it by name, but it's still PlainDog
buddy.name;   // "Buddy" — plain std::string, nothing special
buddy.age;    // 5 — plain int

// Get fields back by name
t.get<std::string>(buddy, "name");    // "Buddy"
t.get<int>(buddy, "age");             // 5
t.get<std::string>(buddy, "breed");   // "Lab"

// Query field-level metadata
t.field("name").has_meta<help_info>();                 // true
t.field("name").meta<help_info>().summary;             // "Dog's name"
t.field("age").has_meta<render_meta>();                 // true
t.field("age").meta<render_meta>().render.style;        // "bold"
t.field("breed").has_meta<help_info>();                // false

// Iterate fields with typed references
t.for_each_field(buddy, [](std::string_view name, auto& value) {
    // value is std::string& or int& — the actual struct member
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
        if (name == "name") value = "Rex";  // mutates buddy.name directly
    }
});
buddy.name;  // "Rex" — changed in-place through for_each_field

// Type mismatches throw, just like typed and dynamic
t.set(buddy, "name", 42);              // throws std::logic_error
t.get<int>(buddy, "name");             // throws std::logic_error
t.set(buddy, "nope", 1);               // throws — unknown field
```

The hybrid path is especially useful when you're working with structs from external code — things you can't (or don't want to) modify.

### One Concept to Rule Them All

All three paths satisfy the `type_definition` concept. Write generic code once:

```cpp
std::string schema_summary(const type_definition auto& t) {
    std::string result = std::string(t.name()) + ": ";
    auto names = t.field_names();
    for (std::size_t i = 0; i < names.size(); ++i) {
        if (i > 0) result += ", ";
        result += names[i];
    }
    return result;
}

// Works with any path:
schema_summary(type_def<Dog>{});           // "Dog: name, age, breed"
schema_summary(event_t);                    // "Event: title, attendees, verbose"
schema_summary(t);                          // "PlainDog: name, age, breed"
```

## Fields

### `field<T>` — The Transparent Value Wrapper

`field<T>` holds a value and gets out of your way:

```cpp
field<int> age;
age = 42;              // assign like a normal int
int years = age;       // read like a normal int
age.value;             // direct access when you need it
```

```cpp
field<std::string> name;
name = "Rex";
name->size();         // operator-> passes through
name->empty();
```

Structs using `field<T>` remain aggregates — designated initializers work:

```cpp
struct Weather {
    field<std::string> city;
    field<int>         days {.value = 7};       // default value
};
Weather weather{};  // city is "", days is 7
```

`field<T>` is used in the typed path. The hybrid path uses plain struct members. The dynamic path stores values internally.

### Field-Level Metadata with `with<>`

Attach domain-specific metadata directly to fields. The metadata types are *yours* — plain structs. `type_def` doesn't define what metadata meta_entryeans, it just carries it:

```cpp
struct cli_meta {
    struct { char short_flag = '\0'; } cli;
};

struct render_meta {
    struct { const char* style = ""; int width = 0; } render;
};
```

How you attach metadata depends on the path:

**Typed path** — metadata baked into the field type via `with<>`:
```cpp
struct CliArgs {
    field<std::string> query;
    field<bool, with<cli_meta>> verbose {
        .with = {{.cli = {.short_flag = 'v'}}}
    };
    field<int, with<cli_meta, render_meta>> limit {
        .with = {{.cli = {.short_flag = 'l'}},
                 {.render = {.style = "bold", .width = 10}}}
    };
};

// Query it
type_def<CliArgs>{}.field("verbose").has_meta<cli_meta>();              // true
type_def<CliArgs>{}.field("verbose").meta<cli_meta>().cli.short_flag;   // 'v'
type_def<CliArgs>{}.field("query").has_meta<cli_meta>();                // false
type_def<CliArgs>{}.field("limit").meta_count<cli_meta>();              // 1
type_def<CliArgs>{}.field("limit").metas<cli_meta>();                   // vector with one element
```

**Hybrid path** — metadata attached at registration with `with<M>({...})`:
```cpp
auto t = type_def<PlainDog>()
    .field(&PlainDog::name, "name",
        with<help_info>({.summary = "Dog's name"}))
    .field(&PlainDog::age, "age",
        with<help_info>({.summary = "Age in years"}),
        with<render_meta>({.render = {.style = "bold"}}));

t.field("name").has_meta<help_info>();                    // true
t.field("name").meta<help_info>().summary;                // "Dog's name"
t.field("age").has_meta<render_meta>();                   // true
t.field("age").meta<render_meta>().render.style;           // "bold"
```

**Dynamic path** — same `with<M>({...})` syntax:
```cpp
auto t = type_def("CLI")
    .field<bool>("verbose", false,
        with<cli_meta>({.cli = {.short_flag = 'v'}}),
        with<render_meta>({.render = {.style = "dim"}}));

t.field("verbose").has_meta<cli_meta>();                  // true
t.field("verbose").meta<cli_meta>().cli.short_flag;       // 'v'
t.field("verbose").has_meta<render_meta>();                // true
t.field("verbose").meta<render_meta>().render.style;       // "dim"
```

Multiple metas of the same type on a single field are supported:

```cpp
// Hybrid
auto t = type_def<PlainDog>()
    .field(&PlainDog::name, "name",
        with<tag_info>({.value = "a"}),
        with<tag_info>({.value = "b"}));

t.field("name").meta_count<tag_info>();  // 2
auto tags = t.field("name").metas<tag_info>();
// tags[0].value == "a", tags[1].value == "b"

// Dynamic
auto event_t = type_def("Event")
    .field<std::string>("title", std::string(""),
        with<tag_info>({.value = "a"}),
        with<tag_info>({.value = "b"}));

event_t.field("title").meta_count<tag_info>();  // 2
```

## Type-Level Metadata with `meta<T>`

Attach metadata to the type itself, not just fields.

**Typed path** — `meta<M>` members on the struct:
```cpp
struct Dog {
    meta<endpoint_info> endpoint{{.path = "/dogs", .method = "POST"}};
    meta<help_info>     help{{.summary = "A good boy"}};
    field<std::string>  name;
    field<int>          age;
};

type_def<Dog> t;
t.has_meta<endpoint_info>();         // true
t.has_meta<help_info>();             // true
t.has_meta<tag_info>();              // false
t.meta<endpoint_info>().path;        // "/dogs"
t.meta<endpoint_info>().method;      // "POST"
t.meta<help_info>().summary;         // "A good boy"
t.meta_count<endpoint_info>();       // 1
```

**Dynamic path** — chained `.meta<M>()` calls:
```cpp
auto t = type_def("Event")
    .meta<endpoint_info>({.path = "/events", .method = "POST"})
    .meta<help_info>({.summary = "An event"})
    .field<std::string>("title");

t.has_meta<endpoint_info>();         // true
t.meta<endpoint_info>().path;        // "/events"
t.meta<help_info>().summary;         // "An event"
t.has_meta<tag_info>();              // false
t.meta<tag_info>();                  // throws std::logic_error — not present
```

**Hybrid path** — metas come from the struct's `meta<M>` members (same as typed):
```cpp
struct MetaDog {
    meta<help_info> help{{.summary = "A dog"}};
    std::string name;
    int age = 0;
};

type_def<MetaDog> t;
t.has_meta<help_info>();             // true
t.meta<help_info>().summary;         // "A dog"
t.has_meta<endpoint_info>();         // false
```

### Multiple metas of the same type

```cpp
struct TaggedThing {
    meta<tag_info> tag1{{.value = "pet"}};
    meta<tag_info> tag2{{.value = "animal"}};
    meta<tag_info> tag3{{.value = "good-boy"}};
    field<std::string> name;
};

type_def<TaggedThing> t;
t.meta_count<tag_info>();            // 3
t.meta<tag_info>().value;            // "pet" — returns the first
auto all = t.metas<tag_info>();      // vector of all three
// all[0].value == "pet"
// all[1].value == "animal"
// all[2].value == "good-boy"

// Dynamic equivalent
auto tagged_t = type_def("Tagged")
    .meta<tag_info>({.value = "first"})
    .meta<tag_info>({.value = "second"})
    .meta<tag_info>({.value = "third"});

tagged_t.meta_count<tag_info>();           // 3
tagged_t.meta<tag_info>().value;           // "first"
tagged_t.metas<tag_info>();                // vector of all three
```

## Querying the Schema

Every `type_def` — typed, hybrid, or dynamic — gives you:

| Method | Returns | Description |
|--------|---------|-------------|
| `name()` | `string_view` | Type name |
| `field_count()` | `size_t` | Number of fields (excludes metas/plain members) |
| `field_names()` | `vector<string>` | Ordered field names |
| `has_field(name)` | `bool` | Field exists? |
| `field(name)` | `field_def` | Field descriptor (throws if not found) |
| `has_meta<M>()` | `bool` | Has metadata of type M? |
| `meta<M>()` | `M` | First metadata of type M (throws if absent, dynamic only) |
| `meta_count<M>()` | `size_t` | Count of metadata of type M |
| `metas<M>()` | `vector<M>` | All metadata of type M |

```cpp
// All three paths — same queries
type_def<Dog>{}.name();                    // "Dog"
type_def<Dog>{}.field_count();             // 3 (meta members excluded)
type_def<Dog>{}.field_names();             // ["name", "age", "breed"]

type_def("Event").field<int>("x").name();   // "Event"
type_def("Event").field<int>("x").field_count();  // 1

auto dog_t = type_def<PlainDog>()
    .field(&PlainDog::name, "name")
    .field(&PlainDog::age, "age");
dog_t.name();                                 // "PlainDog"
dog_t.field_count();                          // 2 (only registered fields)
dog_t.field_names();                          // ["name", "age"]
```

Note that typed `field_count()` excludes `meta<>` members *and* plain (non-`field<>`) members:

```cpp
struct MixedStruct {
    meta<tag_info> tag{{.value = "x"}};  // meta — excluded
    field<std::string> label;             // field — counted
    int counter = 0;                      // plain — excluded
    field<int> score;                     // field — counted
};

type_def<MixedStruct>{}.field_count();     // 2 (label, score)
type_def<MixedStruct>{}.field_names();     // ["label", "score"]
type_def<MixedStruct>{}.has_field("label");    // true
type_def<MixedStruct>{}.has_field("counter");  // false — plain member, not a field
type_def<MixedStruct>{}.has_field("tag");      // false — meta, not a field
```

### Field Descriptors (`field_def`)

`field(name)` returns a descriptor you can query further:

```cpp
// Dynamic — defaults and field-level metas
auto t = type_def("Event")
    .field<std::string>("title")
    .field<int>("attendees", 100,
        with<render_meta>({.render = {.style = "bold", .width = 10}}))
    .field<bool>("verbose", false,
        with<cli_meta>({.cli = {.short_flag = 'v'}}));

auto attendees_field = t.field("attendees");
attendees_field.name();                    // "attendees"
attendees_field.has_default();             // true
attendees_field.default_value<int>();      // 100
attendees_field.has_meta<render_meta>();   // true
attendees_field.meta<render_meta>().render.style;  // "bold"
attendees_field.meta<render_meta>().render.width;  // 10
attendees_field.meta_count<tag_info>();    // 0
attendees_field.metas<tag_info>();         // empty vector

auto title_field = t.field("title");
title_field.has_default();     // false
title_field.default_value<std::string>();  // throws — no default set

// Typed
type_def<CliArgs>{}.field("limit").has_meta<cli_meta>();          // true
type_def<CliArgs>{}.field("limit").meta<cli_meta>().cli.short_flag; // 'l'
type_def<CliArgs>{}.field("query").has_meta<cli_meta>();          // false
type_def<CliArgs>{}.field("query").meta<cli_meta>();              // throws

// Hybrid
auto dog_t = type_def<PlainDog>()
    .field(&PlainDog::name, "name", with<help_info>({.summary = "Dog's name"}));
dog_t.field("name").has_meta<help_info>();          // true
dog_t.field("name").meta<help_info>().summary;      // "Dog's name"
```

Throws for unknown field names:
```cpp
type_def<Dog>{}.field("nonexistent");   // throws std::logic_error
type_def("Event").field("nope");        // throws std::logic_error
```

### type_instance access to schema

A `type_instance` can query its backing type_def through `.type()`:

```cpp
auto t = type_def("Event")
    .meta<endpoint_info>({.path = "/events"})
    .field<std::string>("title")
    .field<int>("count", 100);
auto obj = t.create();

obj.type().name();                          // "Event"
obj.type().field_count();                   // 2
obj.type().field_names();                   // ["title", "count"]
obj.type().has_meta<endpoint_info>();        // true
obj.type().meta<endpoint_info>().path;       // "/events"
obj.type().field("count").has_default();     // true
obj.type().field("count").default_value<int>(); // 100
obj.type().has_field("title");              // true
obj.type().has_field("nope");               // false

// Schema iteration through the instance
obj.type().for_each_field([](field_def field_descriptor) {
    field_descriptor.name();         // "title", "count"
    field_descriptor.has_default();  // false, true
});

obj.type().for_each_meta([](metadata meta_entry) {
    if (meta_entry.is<endpoint_info>())
        meta_entry.as<endpoint_info>().path;  // "/events"
});
```

## Working with Data

### set() and get()

**Typed path** — `t.set(instance, field_name, value)` and `t.get<T>(instance, field_name)`:

```cpp
type_def<Dog> t;
Dog rex;

t.set(rex, "name", std::string("Rex"));
t.set(rex, "age", 3);
t.set(rex, "breed", std::string("Husky"));

t.get<std::string>(rex, "name");    // "Rex"
t.get<int>(rex, "age");             // 3
t.get<std::string>(rex, "breed");   // "Husky"

// Overwrites work
t.set(rex, "name", std::string("Buddy"));
t.get<std::string>(rex, "name");    // "Buddy"
```

**Hybrid path** — exact same API, works on plain struct members:

```cpp
auto t = type_def<PlainDog>()
    .field(&PlainDog::name, "name")
    .field(&PlainDog::age, "age")
    .field(&PlainDog::breed, "breed");

PlainDog rex;

t.set(rex, "name", std::string("Rex"));
t.set(rex, "age", 3);
t.set(rex, "breed", std::string("Husky"));

t.get<std::string>(rex, "name");    // "Rex"
t.get<int>(rex, "age");             // 3
t.get<std::string>(rex, "breed");   // "Husky"

// The struct is still just a PlainDog — you can access members directly
rex.name;     // "Rex"
rex.age;      // 3
rex.breed;    // "Husky"
```

**Dynamic path** — set/get are methods on the instance:

```cpp
auto t = type_def("Event")
    .field<std::string>("title")
    .field<int>("count", 100)
    .field<bool>("active");
auto obj = t.create();

obj.set("title", std::string("Dog Party"));
obj.set("count", 50);
obj.set("active", true);

obj.get<std::string>("title");   // "Dog Party"
obj.get<int>("count");           // 50
obj.get<bool>("active");         // true
```

**const access** works on all paths:

```cpp
// Typed
const Dog& const_rex = rex;
t.get<std::string>(const_rex, "name");    // "Rex" — works on const

// Hybrid
const PlainDog& const_dog = rex;
dog_t.get<std::string>(const_dog, "name");   // works on const

// Dynamic
const auto& const_obj = obj;
const_obj.get<int>("count");              // works on const
```

### Error Handling

Type mismatches throw `std::logic_error` and **do not modify** the field:

```cpp
// Typed
Dog rex;
rex.name = "Original";
t.set(rex, "name", 42);              // throws std::logic_error
rex.name.value;                       // still "Original" — untouched

// Hybrid
PlainDog dog;
dog.name = "Untouched";
dog_t.set(dog, "name", 42);             // throws std::logic_error
dog.name;                             // still "Untouched"

// Dynamic
obj.set("title", 42);                // throws std::logic_error
```

Unknown field names throw:

```cpp
t.set(rex, "nonexistent", 42);       // throws std::logic_error
dog_t.set(dog, "nope", 42);             // throws std::logic_error
obj.set("nope", 1);                  // throws std::logic_error
```

Empty field names throw:

```cpp
t.set(rex, "", 42);                  // throws std::logic_error
obj.set("", 42);                     // throws std::logic_error
obj.get<int>("");                    // throws std::logic_error
```

get with wrong type throws:

```cpp
t.get<int>(rex, "name");             // throws — name is std::string, not int
dog_t.get<std::string>(dog, "age");     // throws — age is int, not string
obj.get<std::string>("count");       // throws — count is int
```

Meta and plain member names are not fields:

```cpp
// Dog has meta<endpoint_info> endpoint and meta<help_info> help
t.set(rex, "endpoint", 42);          // throws — "endpoint" is a meta, not a field
t.get<int>(rex, "help");             // throws — "help" is a meta, not a field

// MixedStruct has a plain int counter
type_def<MixedStruct>{}.set(mixed, "counter", 42);  // throws — "counter" is plain, not a field
```

### Callback-Style get()

For typed and hybrid paths, a callback form gives you the typed reference directly:

```cpp
// Typed
type_def<Dog> t;
Dog rex;
rex.age = 25;

int found_age = 0;
t.get(rex, "age", [&](std::string_view name, auto& value) {
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
        found_age = value;   // read: found_age == 25
        value = 99;          // mutate in-place
    }
});
rex.age.value;  // 99

// Hybrid — same thing, works on plain members
auto dog_t = type_def<PlainDog>()
    .field(&PlainDog::name, "name");
PlainDog dog;
dog.name = "Original";

dog_t.get(dog, "name", [](std::string_view, auto& value) {
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
        value = "Modified";  // mutates dog.name directly
    }
});
dog.name;  // "Modified"

// const callback — read-only
const Dog& const_rex = rex;
t.get(const_rex, "age", [&](std::string_view, const auto& value) {
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
        found_age = value;
    }
});
```

## Iteration

### Iterating Fields with Values

**Typed/Hybrid** — you get real typed references:

```cpp
// Typed
SimpleArgs args;
args.name = "Alice";
args.age = 30;
args.active = true;

type_def<SimpleArgs>{}.for_each_field(args, [](std::string_view name, auto& value) {
    // Visits: name (std::string&), age (int&), active (bool&)
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
        if (name == "name") value = "Changed";  // mutates in-place
    }
});
args.name.value;  // "Changed"

// Hybrid — same pattern, real typed references to plain members
auto t = type_def<PlainDog>()
    .field(&PlainDog::name, "name")
    .field(&PlainDog::age, "age")
    .field(&PlainDog::breed, "breed");

PlainDog rex{"Rex", 3, "Husky"};

std::string found_name;
int found_age = 0;
t.for_each_field(rex, [&](std::string_view name, auto& value) {
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, std::string>) {
        if (name == "name") found_name = value;
    } else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, int>) {
        if (name == "age") found_age = value;
    }
});
// found_name == "Rex", found_age == 3
```

**Dynamic** — via `field_value` wrapper:

```cpp
auto obj = event_t.create();
obj.set("title", std::string("Party"));
obj.set("count", 42);

obj.for_each_field([](std::string_view name, field_value value) {
    if (name == "title")
        value.as<std::string>() = "New Title";  // mutable typed access
    if (name == "count")
        auto count = value.as<int>();                // 42
});

// const iteration
const auto& const_obj = obj;
const_obj.for_each_field([](std::string_view name, const_field_value value) {
    // read-only access
});
```

Iteration skips `meta<>` and plain members — only `field<>` (typed) or registered (hybrid) fields are visited:

```cpp
// Dog has 2 metas + 3 fields — for_each_field visits only the 3 fields
type_def<Dog>{}.for_each_field(rex, [&](std::string_view name, auto&) {
    // Visits: "name", "age", "breed"
    // Does NOT visit: "endpoint", "help" (meta members)
});

// MixedStruct has 1 meta, 2 fields, 1 plain — only fields visited
type_def<MixedStruct>{}.for_each_field(mixed, [&](std::string_view name, auto&) {
    // Visits: "label", "score"
    // Does NOT visit: "tag" (meta), "counter" (plain)
});

// Empty/meta-only — nothing to iterate
type_def<MetaOnly>{}.for_each_field(meta_only, [](auto, auto&) {
    // never called
});
```

### Iterating Field Descriptors (Schema Only)

You can iterate field descriptors without an instance — useful for building UIs, generating help text, etc:

**Typed** — compile-time descriptor with index and meta queries:

```cpp
type_def<CliArgs>{}.for_each_field([](auto descriptor) {
    descriptor.name();         // "query", "verbose", "limit"

    // Compile-time meta queries
    if constexpr (descriptor.template has_meta<cli_meta>()) {
        auto flag = descriptor.template meta<cli_meta>().cli.short_flag;
    }

    // Raw struct member index (useful for advanced reflection)
    if constexpr (requires { descriptor.index(); }) {
        auto idx = descriptor.index();
    }
});
```

**Hybrid** — descriptor with runtime meta queries:

```cpp
auto t = type_def<PlainDog>()
    .field(&PlainDog::name, "name", with<help_info>({.summary = "Dog's name"}))
    .field(&PlainDog::age, "age");

t.for_each_field([](auto descriptor) {
    descriptor.name();                               // "name", "age"
    descriptor.template has_meta<help_info>();        // true, false
});
```

**Dynamic** — `field_def`:

```cpp
event_t.for_each_field([](field_def field_descriptor) {
    field_descriptor.name();                  // "title", "attendees", "verbose"
    field_descriptor.has_default();           // false, true, true
    field_descriptor.has_meta<cli_meta>();    // false, false, true
    if (field_descriptor.has_meta<cli_meta>())
        field_descriptor.meta<cli_meta>().cli.short_flag;  // 'v'
});
```

### Iterating Type-Level Metas

**Typed** — real typed references:

```cpp
type_def<Dog>{}.for_each_meta([](auto& meta_value) {
    using M = std::remove_cvref_t<decltype(meta_value)>;
    if constexpr (std::is_same_v<M, endpoint_info>) {
        // meta_value.path == "/dogs"
        // meta_value.method == "POST"
    } else if constexpr (std::is_same_v<M, help_info>) {
        // meta_value.summary == "A good boy"
    }
});

// With an instance — reads from that specific instance's meta members
Dog rex;
type_def<Dog>{}.for_each_meta(rex, [](auto& meta_value) {
    // same typed access
});

// Count
int count = 0;
type_def<Dog>{}.for_each_meta([&](auto&) { ++count; });
// count == 2 (endpoint_info + help_info)
```

**Hybrid** — same pattern for structs with `meta<>` members:

```cpp
struct MetaDog {
    meta<help_info> help{{.summary = "A dog"}};
    std::string name;
    int age = 0;
};

type_def<MetaDog>{}.for_each_meta([](auto& meta_value) {
    using M = std::remove_cvref_t<decltype(meta_value)>;
    if constexpr (std::is_same_v<M, help_info>) {
        // meta_value.summary == "A dog"
    }
});

// Plain struct with no metas — nothing to iterate
auto t = type_def<PlainDog>().field(&PlainDog::name, "name");
int count = 0;
t.for_each_meta([&](auto&) { ++count; });
// count == 0
```

**Dynamic** — `metadata` wrapper with type checking:

```cpp
auto t = type_def("Event")
    .meta<endpoint_info>({.path = "/events", .method = "POST"})
    .meta<help_info>({.summary = "An event"});

t.for_each_meta([](metadata meta_entry) {
    // Type checking
    meta_entry.is<endpoint_info>();     // true on first iteration
    meta_entry.is<help_info>();         // true on second iteration

    // Typed access — throws if wrong type
    if (meta_entry.is<endpoint_info>()) {
        auto& endpoint = meta_entry.as<endpoint_info>();
        // endpoint.path == "/events", endpoint.method == "POST"
    }

    // Safe access — returns nullptr if wrong type
    auto* help = meta_entry.try_as<help_info>();
    if (help) {
        // help->summary == "An event"
    }

    // Wrong type throws
    // meta_entry.as<tag_info>();  // throws std::logic_error if meta_entry is not tag_info
});

// Multiple metas of the same type
auto tagged = type_def("Tagged")
    .meta<tag_info>({.value = "a"})
    .meta<tag_info>({.value = "b"})
    .meta<tag_info>({.value = "c"});

std::vector<std::string> values;
tagged.for_each_meta([&](metadata meta_entry) {
    if (meta_entry.is<tag_info>())
        values.push_back(meta_entry.as<tag_info>().value);
});
// values == ["a", "b", "c"]
```

**type_instance** — meta iteration through `.type()`:

```cpp
auto t = type_def("Event")
    .meta<endpoint_info>({.path = "/events"})
    .meta<help_info>({.summary = "An event"})
    .field<int>("x");
auto obj = t.create();

obj.type().for_each_meta([](metadata meta_entry) {
    if (meta_entry.is<endpoint_info>())
        meta_entry.as<endpoint_info>().path;  // "/events"
});
```

## type_instance — Runtime Objects

Dynamic type_defs produce instances via `create()`:

```cpp
auto t = type_def("Event")
    .field<std::string>("title")
    .field<int>("count", 100);

auto obj = t.create();              // or: type_instance obj(t);

// Defaults are populated
obj.get<int>("count");              // 100 (from default)
obj.get<std::string>("title");      // "" (default-constructed, no explicit default)

// Set and get
obj.set("title", std::string("Party"));
obj.get<std::string>("title");      // "Party"

// Field existence
obj.has("title");                   // true
obj.has("count");                   // true
obj.has("nope");                    // false
obj.has("");                        // false
obj.has("Count");                   // false — case-sensitive

// Access the backing type_def
obj.type().name();                  // "Event"
obj.type().field_count();           // 2
obj.type().has_meta<endpoint_info>(); // depends on type_def
```

### Multiple instances are independent

```cpp
auto t = type_def("Event")
    .field<std::string>("title", std::string("Default"))
    .field<int>("count", 0);

auto a = t.create();
auto b = t.create();

a.set("title", std::string("Party A"));
a.set("count", 10);

b.set("title", std::string("Party B"));
b.set("count", 20);

a.get<std::string>("title");  // "Party A"
a.get<int>("count");           // 10
b.get<std::string>("title");  // "Party B"
b.get<int>("count");           // 20
```

## create() — Instance Creation

All three paths support `create()`:

```cpp
// Typed — returns a default-constructed instance of the struct
type_def<Dog> t;
Dog rex = t.create();
rex.name.value;    // "" (default)
rex.age.value;     // 0 (default)

// The instance is fully mutable
t.set(rex, "name", std::string("Buddy"));
t.get<std::string>(rex, "name");  // "Buddy"

// Hybrid — returns a default-constructed instance of the plain struct
auto dog_t = type_def<PlainDog>()
    .field(&PlainDog::name, "name")
    .field(&PlainDog::age, "age");
PlainDog dog = dog_t.create();
dog.name;    // "" (default)
dog.age;     // 0 (default)

dog_t.set(dog, "name", std::string("Rex"));
dog_t.get<std::string>(dog, "name");  // "Rex"

// Dynamic — returns a type_instance
auto event_t = type_def("Event")
    .field<std::string>("title")
    .field<int>("count", 100);
auto obj = event_t.create();
obj.get<int>("count");  // 100 (from default)
```

## Safety

`type_def` is strict. If you do something wrong, you get a `std::logic_error` with a clear message:

- **Unknown field**: `set()`, `get()`, `field()` throw for names that don't exist
- **Type mismatch**: `set()` with wrong type throws, value is **not modified**
- **Empty field name**: throws on all operations
- **Absent meta**: `meta<M>()` throws on dynamic path when M isn't present
- **Meta/plain member names**: `set()`, `get()`, `has_field()` correctly ignore non-field members — you can't accidentally access a `meta<>` or plain member by name as if it were a field
- **Case-sensitive**: `has_field("Name")` is not the same as `has_field("name")`

```cpp
// All of these throw std::logic_error:
t.set(rex, "nonexistent", 42);       // unknown field
t.set(rex, "name", 42);              // type mismatch (string field, int value)
t.set(rex, "", 42);                  // empty field name
t.get<int>(rex, "name");             // type mismatch on get
t.set(rex, "endpoint", 42);          // "endpoint" is a meta, not a field
type_def("X").field<int>("x").meta<tag_info>();  // absent meta on dynamic
```
