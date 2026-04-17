# Validation & Parse Result Design

> Design document for validation and deserialization error handling in the `type_def` system.
> Status: **Design phase** — not yet implemented.

---

## Philosophy

- Validation is an **opt-in feature**, not a gate. Objects are always fully constructed.
- Validation is **stateless** — results are computed on demand, never cached on the instance.
- `from_json` always returns a **fully populated object** plus a report of what happened.
- For **validation failures**, the actual parsed value is preserved (not zeroed out).
- For **parse failures** (e.g., JSON string where int expected), the field gets its default value.
- Inspired by Pydantic, but we do better: Pydantic gives you the object OR errors. We give you **both**.

---

## `valid()` and `validate()` — Runtime Validation

Two functions, all four paths. Stateless — nothing stored on the instance.

### Signatures

```cpp
// type_instance (dynamic)
obj.valid();       // → bool
obj.validate();    // → validation_result

// typed
t.valid(rex);      // → bool
t.validate(rex);   // → validation_result

// hybrid
dog_t.valid(rex);      // → bool
dog_t.validate(rex);   // → validation_result
```

### `valid()` vs `validate()`

- `valid()` — returns `bool`. Can short-circuit on first failure (fast path).
- `validate()` — returns `validation_result`. Collects ALL errors.

### `validation_result`

```cpp
struct validation_error {
    std::string path;         // "age" or "address.zip" for nested
    std::string message;      // runtime message, e.g. "5 exceeds maximum of 3"
    std::string constraint;   // validator name, e.g. "max_length"
};

struct validation_result {
    explicit operator bool() const;  // true if no errors
    bool ok() const;

    const std::vector<validation_error>& errors() const;
    std::size_t error_count() const;

    auto begin() const;  // iterate errors
    auto end() const;
};
```

---

## `from_json` — Deserialization with Reporting

### Return Type: `parse_result<T>`

NOT `std::expected`. We always return the object AND the report.

```cpp
template <typename T>
struct parse_result {
    T value;  // always fully populated (best-effort)

    // --- Error checks ---
    bool valid() const;                // no validation errors
    bool has_extra_keys() const;       // JSON had keys not in schema
    bool has_missing_fields() const;   // schema had fields not in JSON

    // --- Data access ---
    std::vector<std::string> extra_keys() const;
    std::vector<std::string> missing_fields() const;
    std::vector<validation_error> validation_errors() const;

    // --- Value access ---
    T& operator*();
    const T& operator*() const;
    T* operator->();

    // throws if validation errors exist
    T& checked_value();
};
```

### Three categories of information

| Category | What it means | Makes `valid()` false? |
|----------|--------------|----------------------|
| **extra_keys** | JSON had keys that don't match any field in the schema | No |
| **missing_fields** | Schema has fields that weren't present in the JSON (got defaults) | No |
| **validation_errors** | Validators failed on populated values | **Yes** |

`extra_keys` and `missing_fields` are **informational**. Only validation errors affect `valid()`.

### Signatures

```cpp
// Typed path
auto result = from_json<Dog>(json_str);
// parse_result<Dog>

// Dynamic path
auto result = dog_t.from_json(json_str);
// parse_result<type_instance>

// Hybrid path
auto result = dog_t.from_json<PlainDog>(json_str);
// parse_result<PlainDog>
```

### When does validation run?

**By default, validation does NOT run during `from_json`.** The parse result tracks extra keys and missing fields, but validators are not executed unless requested.

Validation runs during `from_json` only if:
- `parse_options::require_valid = true`
- `parse_options::strict = true`

Otherwise, call `.valid()` or `.validate()` on the result or instance afterward.

---

## `parse_options` — Configurable Strictness

```cpp
struct parse_options {
    bool reject_extra_keys  = false;  // throw if JSON has unknown keys
    bool require_all_fields = false;  // throw if schema fields missing from JSON
    bool require_valid      = false;  // run validators and throw if any fail
    bool strict             = false;  // sets all three to true
};
```

### Usage

```cpp
// Default — no throws, just data
auto r = dog_t.from_json(json_str);

// Strict — any issue throws
auto r = dog_t.from_json(json_str, {.strict = true});

// Pick and choose
auto r = dog_t.from_json(json_str, {.require_valid = true});
```

### Exception on strict failure

```cpp
try {
    auto r = dog_t.from_json(json_str, {.strict = true});
} catch (const parse_error& e) {
    e.extra_keys();
    e.missing_fields();
    e.validation_errors();
}
```

The exception carries the same structured data as `parse_result`.

---

## Field Population Rules

| Scenario | What goes in the field |
|----------|----------------------|
| JSON value parses successfully | The parsed value |
| JSON value fails to parse (type mismatch) | Field's default value |
| JSON value parses but fails validation | **The actual parsed value** (preserved) |
| Field not present in JSON | Field's default value |

Validation failures preserve the data. The caller can see what the bad value was.
Parse failures have no choice — there's no valid value to store, so the default is used.

---

## Attaching Validators to Fields (Typed Path)

Validators live on `field<T>` as a `.validators` member, using the `validators()` helper:

```cpp
struct Dog {
    field<std::string, with<posix_meta>> name {
        .with = {{.posix = {.short_flag = 'n'}}},
        .value = "Rex",
        .validators = validators(not_empty{}, max_length{50})
    };
    field<int> age {
        .value = 0,
        .validators = validators(positive{}, less_than{30})
    };
    field<std::string> breed;  // no validators
};
```

### Validator contract

A validator is any struct with:

```cpp
std::optional<std::string> operator()(const T& value) const;
```

- Return `std::nullopt` → valid
- Return a string → the error message (runtime, can include actual values)

### Example validators

```cpp
struct not_empty {
    std::optional<std::string> operator()(const std::string& v) const {
        if (!v.empty()) return std::nullopt;
        return "must not be empty";
    }
};

struct max_length {
    std::size_t limit;
    std::optional<std::string> operator()(const std::string& v) const {
        if (v.size() <= limit) return std::nullopt;
        return std::format("length {} exceeds maximum of {}", v.size(), limit);
    }
};

struct positive {
    std::optional<std::string> operator()(auto v) const {
        if (v > 0) return std::nullopt;
        return std::format("{} must be positive", v);
    }
};

struct less_than {
    int limit;
    std::optional<std::string> operator()(auto v) const {
        if (v < limit) return std::nullopt;
        return std::format("{} must be less than {}", v, limit);
    }
};
```

### Dynamic path

```cpp
auto dog_t = type_def("Dog")
    .field<std::string>("name",
        validators(not_empty{}, max_length{50}))
    .field<int>("age", 0,
        validators(positive{}, less_than{30}))
    .field<std::string>("breed");
```

### Hybrid path

```cpp
auto dog_t = type_def<PlainDog>()
    .field(&PlainDog::name, "name",
        validators(not_empty{}, max_length{50}))
    .field(&PlainDog::age, "age",
        validators(positive{}));
```
