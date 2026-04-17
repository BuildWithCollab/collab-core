#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <nlohmann/json.hpp>

#include <ankerl/unordered_dense.h>

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

import collab.core;

using namespace collab::model;
using json = nlohmann::json;

// ── Forward declarations for struct_info fallbacks ──────────────────────

struct SimpleArgs;
struct WithDefaults;
struct Address;
struct Person;
struct TaggedItem;
struct MaybeNickname;
struct CliArgs;
struct EnumStruct;
struct MultiEnumStruct;
struct MixedStruct;
struct Team;
struct Config;
struct Labels;
struct Endpoint;
struct ServiceMap;
struct EmptyConfig;
struct TagSet;
struct IdSet;
struct DenseMapStruct;
struct DenseSetStruct;
struct Measurement;
struct FloatStruct;
struct BigNumbers;
struct Inner;
struct Outer;

#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::model::struct_info<SimpleArgs>() {
    return collab::model::field_info<SimpleArgs>("name", "age", "active");
}

template <>
constexpr auto collab::model::struct_info<WithDefaults>() {
    return collab::model::field_info<WithDefaults>("city", "days");
}

template <>
constexpr auto collab::model::struct_info<Address>() {
    return collab::model::field_info<Address>("street", "zip");
}

template <>
constexpr auto collab::model::struct_info<Person>() {
    return collab::model::field_info<Person>("name", "address");
}

template <>
constexpr auto collab::model::struct_info<TaggedItem>() {
    return collab::model::field_info<TaggedItem>("title", "tags");
}

template <>
constexpr auto collab::model::struct_info<MaybeNickname>() {
    return collab::model::field_info<MaybeNickname>("name", "nickname");
}

template <>
constexpr auto collab::model::struct_info<CliArgs>() {
    return collab::model::field_info<CliArgs>("query", "verbose");
}

template <>
constexpr auto collab::model::struct_info<MixedStruct>() {
    return collab::model::field_info<MixedStruct>("visible", "internal_counter", "score");
}

template <>
constexpr auto collab::model::struct_info<Team>() {
    return collab::model::field_info<Team>("team_name", "members");
}

template <>
constexpr auto collab::model::struct_info<Config>() {
    return collab::model::field_info<Config>("name", "settings");
}

template <>
constexpr auto collab::model::struct_info<Labels>() {
    return collab::model::field_info<Labels>("tags");
}

template <>
constexpr auto collab::model::struct_info<Endpoint>() {
    return collab::model::field_info<Endpoint>("url", "port");
}

template <>
constexpr auto collab::model::struct_info<ServiceMap>() {
    return collab::model::field_info<ServiceMap>("services");
}

template <>
constexpr auto collab::model::struct_info<EmptyConfig>() {
    return collab::model::field_info<EmptyConfig>("settings");
}

template <>
constexpr auto collab::model::struct_info<TagSet>() {
    return collab::model::field_info<TagSet>("tags");
}

template <>
constexpr auto collab::model::struct_info<IdSet>() {
    return collab::model::field_info<IdSet>("ids");
}

template <>
constexpr auto collab::model::struct_info<DenseMapStruct>() {
    return collab::model::field_info<DenseMapStruct>("scores");
}

template <>
constexpr auto collab::model::struct_info<DenseSetStruct>() {
    return collab::model::field_info<DenseSetStruct>("names");
}

template <>
constexpr auto collab::model::struct_info<Measurement>() {
    return collab::model::field_info<Measurement>("unit", "value");
}

template <>
constexpr auto collab::model::struct_info<FloatStruct>() {
    return collab::model::field_info<FloatStruct>("weight");
}

template <>
constexpr auto collab::model::struct_info<BigNumbers>() {
    return collab::model::field_info<BigNumbers>("signed_big", "unsigned_big");
}

template <>
constexpr auto collab::model::struct_info<Inner>() {
    return collab::model::field_info<Inner>("x");
}

template <>
constexpr auto collab::model::struct_info<Outer>() {
    return collab::model::field_info<Outer>("name", "extra");
}

template <>
constexpr auto collab::model::struct_info<EnumStruct>() {
    return collab::model::field_info<EnumStruct>("method", "name");
}

template <>
constexpr auto collab::model::struct_info<MultiEnumStruct>() {
    return collab::model::field_info<MultiEnumStruct>("color", "method", "label");
}
#endif

// ── Test structs ─────────────────────────────────────────────────────────

struct SimpleArgs {
    field<std::string> name;
    field<int>         age;
    field<bool>        active;
};

struct WithDefaults {
    field<std::string> city    {.value = "Portland"};
    field<int>         days    {.value = 7};
};

struct Address {
    field<std::string> street;
    field<std::string> zip;
};

struct Person {
    field<std::string> name;
    field<Address>     address;
};

struct TaggedItem {
    field<std::string>              title;
    field<std::vector<std::string>> tags;
};

struct MaybeNickname {
    field<std::string>                name;
    field<std::optional<std::string>> nickname;
};

struct posix_options {
    char short_flag = '\0';
    bool from_stdin = false;
};

struct posix_meta { posix_options posix{}; };

struct CliArgs {
    field<std::string, with<posix_meta>> query {
        .with = {{.posix = {.short_flag = 'q', .from_stdin = true}}}
    };
    field<bool, with<posix_meta>> verbose {
        .with = {{.posix = {.short_flag = 'v'}}}
    };
};

struct MixedStruct {
    field<std::string> visible;
    int                internal_counter = 0;
    field<int>         score;
};

struct Team {
    field<std::string>             team_name;
    field<std::vector<SimpleArgs>> members;
};

struct Config {
    field<std::string>                name;
    field<std::map<std::string, int>> settings;
};

struct Labels {
    field<std::unordered_map<std::string, std::string>> tags;
};

struct Endpoint {
    field<std::string> url;
    field<int>         port;
};

struct ServiceMap {
    field<std::map<std::string, Endpoint>> services;
};

struct EmptyConfig {
    field<std::map<std::string, int>> settings;
};

struct TagSet {
    field<std::set<std::string>> tags;
};

struct IdSet {
    field<std::unordered_set<int>> ids;
};

struct DenseMapStruct {
    field<ankerl::unordered_dense::map<std::string, int>> scores;
};

struct DenseSetStruct {
    field<ankerl::unordered_dense::set<std::string>> names;
};

struct Measurement {
    field<std::string> unit;
    field<double>      value;
};

struct FloatStruct {
    field<float> weight;
};

struct BigNumbers {
    field<int64_t>  signed_big;
    field<uint64_t> unsigned_big;
};

struct Inner {
    field<int> x;
};

struct Outer {
    field<std::string>          name;
    field<std::optional<Inner>> extra;
};

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — primitives
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: primitive fields", "[from_json][typed]") {
    auto j = json{{"name", "Alice"}, {"age", 30}, {"active", true}};
    auto args = from_json<SimpleArgs>(j);

    REQUIRE(args.name.value == "Alice");
    REQUIRE(args.age.value == 30);
    REQUIRE(args.active.value == true);
}

TEST_CASE("typed from_json: preserves struct defaults for missing keys", "[from_json][typed]") {
    auto j = json::object();
    auto w = from_json<WithDefaults>(j);

    REQUIRE(w.city.value == "Portland");
    REQUIRE(w.days.value == 7);
}

TEST_CASE("typed from_json: partial overlay — some keys present, some missing", "[from_json][typed]") {
    auto j = json{{"city", "Seattle"}};
    auto w = from_json<WithDefaults>(j);

    REQUIRE(w.city.value == "Seattle");
    REQUIRE(w.days.value == 7);
}

TEST_CASE("typed from_json: extra keys are silently ignored", "[from_json][typed]") {
    auto j = json{{"name", "Bob"}, {"age", 25}, {"active", false},
                   {"unknown_field", "ignored"}, {"another", 999}};
    auto args = from_json<SimpleArgs>(j);

    REQUIRE(args.name.value == "Bob");
    REQUIRE(args.age.value == 25);
    REQUIRE(args.active.value == false);
}

TEST_CASE("typed from_json: empty JSON object gives all defaults", "[from_json][typed]") {
    auto args = from_json<SimpleArgs>(json::object());

    REQUIRE(args.name.value.empty());
    REQUIRE(args.age.value == 0);
    REQUIRE(args.active.value == false);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — nested structs
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: nested reflected struct", "[from_json][typed][nested]") {
    auto j = json{
        {"name", "Bob"},
        {"address", {{"street", "123 Main St"}, {"zip", "97201"}}}
    };
    auto p = from_json<Person>(j);

    REQUIRE(p.name.value == "Bob");
    REQUIRE(p.address.value.street.value == "123 Main St");
    REQUIRE(p.address.value.zip.value == "97201");
}

TEST_CASE("typed from_json: nested struct with missing inner keys", "[from_json][typed][nested]") {
    auto j = json{
        {"name", "Bob"},
        {"address", {{"street", "123 Main St"}}}
    };
    auto p = from_json<Person>(j);

    REQUIRE(p.address.value.street.value == "123 Main St");
    REQUIRE(p.address.value.zip.value.empty());
}

TEST_CASE("typed from_json: missing nested object entirely", "[from_json][typed][nested]") {
    auto j = json{{"name", "Bob"}};
    auto p = from_json<Person>(j);

    REQUIRE(p.name.value == "Bob");
    REQUIRE(p.address.value.street.value.empty());
    REQUIRE(p.address.value.zip.value.empty());
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — vectors
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: vector of strings", "[from_json][typed][vector]") {
    auto j = json{{"title", "My Post"}, {"tags", {"c++", "modules", "reflection"}}};
    auto item = from_json<TaggedItem>(j);

    REQUIRE(item.title.value == "My Post");
    REQUIRE(item.tags.value.size() == 3);
    REQUIRE(item.tags.value[0] == "c++");
    REQUIRE(item.tags.value[1] == "modules");
    REQUIRE(item.tags.value[2] == "reflection");
}

TEST_CASE("typed from_json: empty vector", "[from_json][typed][vector]") {
    auto j = json{{"title", "Empty"}, {"tags", json::array()}};
    auto item = from_json<TaggedItem>(j);

    REQUIRE(item.tags.value.empty());
}

TEST_CASE("typed from_json: vector of nested reflected structs", "[from_json][typed][vector]") {
    auto j = json{
        {"team_name", "Pirates"},
        {"members", {
            {{"name", "Alice"}, {"age", 30}, {"active", true}},
            {{"name", "Bob"}, {"age", 25}, {"active", false}}
        }}
    };
    auto t = from_json<Team>(j);

    REQUIRE(t.team_name.value == "Pirates");
    REQUIRE(t.members.value.size() == 2);
    REQUIRE(t.members.value[0].name.value == "Alice");
    REQUIRE(t.members.value[0].age.value == 30);
    REQUIRE(t.members.value[0].active.value == true);
    REQUIRE(t.members.value[1].name.value == "Bob");
    REQUIRE(t.members.value[1].age.value == 25);
    REQUIRE(t.members.value[1].active.value == false);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — optionals
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: optional present", "[from_json][typed][optional]") {
    auto j = json{{"name", "Alice"}, {"nickname", "Ali"}};
    auto m = from_json<MaybeNickname>(j);

    REQUIRE(m.name.value == "Alice");
    REQUIRE(m.nickname.value.has_value());
    REQUIRE(*m.nickname.value == "Ali");
}

TEST_CASE("typed from_json: optional null", "[from_json][typed][optional]") {
    auto j = json{{"name", "Bob"}, {"nickname", nullptr}};
    auto m = from_json<MaybeNickname>(j);

    REQUIRE(m.name.value == "Bob");
    REQUIRE(!m.nickname.value.has_value());
}

TEST_CASE("typed from_json: optional missing key", "[from_json][typed][optional]") {
    auto j = json{{"name", "Bob"}};
    auto m = from_json<MaybeNickname>(j);

    REQUIRE(m.name.value == "Bob");
    REQUIRE(!m.nickname.value.has_value());
}

TEST_CASE("typed from_json: optional nested struct — present", "[from_json][typed][optional][nested]") {
    auto j = json{{"name", "test"}, {"extra", {{"x", 42}}}};
    auto o = from_json<Outer>(j);

    REQUIRE(o.name.value == "test");
    REQUIRE(o.extra.value.has_value());
    REQUIRE(o.extra.value->x.value == 42);
}

TEST_CASE("typed from_json: optional nested struct — null", "[from_json][typed][optional][nested]") {
    auto j = json{{"name", "test"}, {"extra", nullptr}};
    auto o = from_json<Outer>(j);

    REQUIRE(o.name.value == "test");
    REQUIRE(!o.extra.value.has_value());
}

TEST_CASE("typed from_json: optional nested struct — missing", "[from_json][typed][optional][nested]") {
    auto j = json{{"name", "test"}};
    auto o = from_json<Outer>(j);

    REQUIRE(!o.extra.value.has_value());
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — maps
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: std::map<string, int>", "[from_json][typed][map]") {
    auto j = json{{"name", "my-app"}, {"settings", {{"timeout", 30}, {"retries", 3}}}};
    auto c = from_json<Config>(j);

    REQUIRE(c.name.value == "my-app");
    REQUIRE(c.settings.value.at("timeout") == 30);
    REQUIRE(c.settings.value.at("retries") == 3);
}

TEST_CASE("typed from_json: std::unordered_map<string, string>", "[from_json][typed][map]") {
    auto j = json{{"tags", {{"env", "prod"}, {"region", "us-west"}}}};
    auto l = from_json<Labels>(j);

    REQUIRE(l.tags.value.at("env") == "prod");
    REQUIRE(l.tags.value.at("region") == "us-west");
}

TEST_CASE("typed from_json: map of reflected structs", "[from_json][typed][map]") {
    auto j = json{{"services", {
        {"api", {{"url", "https://api.example.com"}, {"port", 443}}},
        {"db",  {{"url", "localhost"}, {"port", 5432}}}
    }}};
    auto sm = from_json<ServiceMap>(j);

    REQUIRE(sm.services.value.at("api").url.value == "https://api.example.com");
    REQUIRE(sm.services.value.at("api").port.value == 443);
    REQUIRE(sm.services.value.at("db").url.value == "localhost");
    REQUIRE(sm.services.value.at("db").port.value == 5432);
}

TEST_CASE("typed from_json: empty map", "[from_json][typed][map]") {
    auto j = json{{"settings", json::object()}};
    auto ec = from_json<EmptyConfig>(j);

    REQUIRE(ec.settings.value.empty());
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — sets
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: std::set<string>", "[from_json][typed][set]") {
    auto j = json{{"tags", {"alpha", "beta", "gamma"}}};
    auto ts = from_json<TagSet>(j);

    REQUIRE(ts.tags.value.size() == 3);
    REQUIRE(ts.tags.value.count("alpha") == 1);
    REQUIRE(ts.tags.value.count("beta") == 1);
    REQUIRE(ts.tags.value.count("gamma") == 1);
}

TEST_CASE("typed from_json: std::unordered_set<int>", "[from_json][typed][set]") {
    auto j = json{{"ids", {10, 20, 30}}};
    auto is = from_json<IdSet>(j);

    REQUIRE(is.ids.value.size() == 3);
    REQUIRE(is.ids.value.count(10) == 1);
    REQUIRE(is.ids.value.count(20) == 1);
    REQUIRE(is.ids.value.count(30) == 1);
}

TEST_CASE("typed from_json: empty set", "[from_json][typed][set]") {
    auto j = json{{"tags", json::array()}};
    auto ts = from_json<TagSet>(j);

    REQUIRE(ts.tags.value.empty());
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — ankerl::unordered_dense
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: ankerl::unordered_dense::map", "[from_json][typed][dense]") {
    auto j = json{{"scores", {{"alice", 100}, {"bob", 85}}}};
    auto dm = from_json<DenseMapStruct>(j);

    REQUIRE(dm.scores.value.at("alice") == 100);
    REQUIRE(dm.scores.value.at("bob") == 85);
}

TEST_CASE("typed from_json: ankerl::unordered_dense::set", "[from_json][typed][dense]") {
    auto j = json{{"names", {"x", "y", "z"}}};
    auto ds = from_json<DenseSetStruct>(j);

    REQUIRE(ds.names.value.size() == 3);
    REQUIRE(ds.names.value.count("x") == 1);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — numeric types
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: double field", "[from_json][typed][numeric]") {
    auto j = json{{"unit", "celsius"}, {"value", 72.5}};
    auto m = from_json<Measurement>(j);

    REQUIRE(m.unit.value == "celsius");
    REQUIRE(m.value.value == 72.5);
}

TEST_CASE("typed from_json: float field", "[from_json][typed][numeric]") {
    auto j = json{{"weight", 3.14}};
    auto fs = from_json<FloatStruct>(j);

    REQUIRE(fs.weight.value == Catch::Approx(3.14f));
}

TEST_CASE("typed from_json: int64 and uint64", "[from_json][typed][numeric]") {
    auto j = json{{"signed_big", 9000000000LL}, {"unsigned_big", 18000000000ULL}};
    auto bn = from_json<BigNumbers>(j);

    REQUIRE(bn.signed_big.value == 9'000'000'000LL);
    REQUIRE(bn.unsigned_big.value == 18'000'000'000ULL);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — extensions (with<>) are untouched
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: extensions untouched — only value deserialized", "[from_json][typed][with]") {
    auto j = json{{"query", "hello world"}, {"verbose", true}};
    auto args = from_json<CliArgs>(j);

    REQUIRE(args.query.value == "hello world");
    REQUIRE(args.verbose.value == true);
    REQUIRE(args.query.with.posix.short_flag == 'q');
    REQUIRE(args.query.with.posix.from_stdin == true);
    REQUIRE(args.verbose.with.posix.short_flag == 'v');
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — mixed struct (skips non-field members)
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: skips non-field members", "[from_json][typed][mixed]") {
    auto j = json{{"visible", "hello"}, {"score", 42}, {"internal_counter", 999}};
    auto ms = from_json<MixedStruct>(j);

    REQUIRE(ms.visible.value == "hello");
    REQUIRE(ms.score.value == 42);
    REQUIRE(ms.internal_counter == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — from string
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: from JSON string", "[from_json][typed][string]") {
    auto args = from_json<SimpleArgs>(std::string(R"({"name":"Alice","age":30,"active":true})"));

    REQUIRE(args.name.value == "Alice");
    REQUIRE(args.age.value == 30);
    REQUIRE(args.active.value == true);
}

TEST_CASE("typed from_json: from pretty JSON string", "[from_json][typed][string]") {
    std::string pretty = R"({
        "name": "Bob",
        "age": 25,
        "active": false
    })";
    auto args = from_json<SimpleArgs>(pretty);

    REQUIRE(args.name.value == "Bob");
    REQUIRE(args.age.value == 25);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — round-trip (to_json → from_json)
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: round-trip simple struct", "[from_json][typed][roundtrip]") {
    SimpleArgs original;
    original.name = "Alice";
    original.age = 30;
    original.active = true;

    auto j = to_json(original);
    auto restored = from_json<SimpleArgs>(j);

    REQUIRE(restored.name.value == original.name.value);
    REQUIRE(restored.age.value == original.age.value);
    REQUIRE(restored.active.value == original.active.value);
}

TEST_CASE("typed from_json: round-trip nested struct", "[from_json][typed][roundtrip]") {
    Person original;
    original.name = "Bob";
    original.address.value.street = "123 Main";
    original.address.value.zip = "97201";

    auto j = to_json(original);
    auto restored = from_json<Person>(j);

    REQUIRE(restored.name.value == "Bob");
    REQUIRE(restored.address.value.street.value == "123 Main");
    REQUIRE(restored.address.value.zip.value == "97201");
}

TEST_CASE("typed from_json: round-trip vector of structs", "[from_json][typed][roundtrip]") {
    Team original;
    original.team_name = "Pirates";
    SimpleArgs a1; a1.name = "Alice"; a1.age = 30; a1.active = true;
    SimpleArgs a2; a2.name = "Bob"; a2.age = 25; a2.active = false;
    original.members.value = {a1, a2};

    auto j = to_json(original);
    auto restored = from_json<Team>(j);

    REQUIRE(restored.team_name.value == "Pirates");
    REQUIRE(restored.members.value.size() == 2);
    REQUIRE(restored.members.value[0].name.value == "Alice");
    REQUIRE(restored.members.value[1].name.value == "Bob");
}

TEST_CASE("typed from_json: round-trip map of structs", "[from_json][typed][roundtrip]") {
    ServiceMap original;
    Endpoint api; api.url = "https://api.example.com"; api.port = 443;
    Endpoint db; db.url = "localhost"; db.port = 5432;
    original.services.value = {{"api", api}, {"db", db}};

    auto j = to_json(original);
    auto restored = from_json<ServiceMap>(j);

    REQUIRE(restored.services.value.at("api").url.value == "https://api.example.com");
    REQUIRE(restored.services.value.at("api").port.value == 443);
    REQUIRE(restored.services.value.at("db").port.value == 5432);
}

TEST_CASE("typed from_json: round-trip optional present", "[from_json][typed][roundtrip]") {
    MaybeNickname original;
    original.name = "Alice";
    original.nickname.value = "Ali";

    auto j = to_json(original);
    auto restored = from_json<MaybeNickname>(j);

    REQUIRE(restored.nickname.value.has_value());
    REQUIRE(*restored.nickname.value == "Ali");
}

TEST_CASE("typed from_json: round-trip optional absent", "[from_json][typed][roundtrip]") {
    MaybeNickname original;
    original.name = "Bob";

    auto j = to_json(original);
    auto restored = from_json<MaybeNickname>(j);

    REQUIRE(!restored.nickname.value.has_value());
}

// ═════════════════════════════════════════════════════════════════════════
// Typed from_json — error cases
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: throws on non-object JSON", "[from_json][typed][throw]") {
    REQUIRE_THROWS_AS(from_json<SimpleArgs>(json(42)), std::logic_error);
    REQUIRE_THROWS_AS(from_json<SimpleArgs>(json("string")), std::logic_error);
    REQUIRE_THROWS_AS(from_json<SimpleArgs>(json::array()), std::logic_error);
    REQUIRE_THROWS_AS(from_json<SimpleArgs>(json(true)), std::logic_error);
}

TEST_CASE("typed from_json: throws on type mismatch — string field gets int", "[from_json][typed][throw]") {
    auto j = json{{"name", 42}, {"age", 30}, {"active", true}};
    REQUIRE_THROWS_AS(from_json<SimpleArgs>(j), std::logic_error);
}

TEST_CASE("typed from_json: throws on type mismatch — int field gets string", "[from_json][typed][throw]") {
    auto j = json{{"name", "Alice"}, {"age", "thirty"}, {"active", true}};
    REQUIRE_THROWS_AS(from_json<SimpleArgs>(j), std::logic_error);
}

TEST_CASE("typed from_json: throws on type mismatch — bool field gets int", "[from_json][typed][throw]") {
    auto j = json{{"name", "Alice"}, {"age", 30}, {"active", 1}};
    REQUIRE_THROWS_AS(from_json<SimpleArgs>(j), std::logic_error);
}

TEST_CASE("typed from_json: throws on type mismatch — vector field gets object", "[from_json][typed][throw]") {
    auto j = json{{"title", "test"}, {"tags", {{"not", "an array"}}}};
    REQUIRE_THROWS_AS(from_json<TaggedItem>(j), std::logic_error);
}

TEST_CASE("typed from_json: throws on type mismatch — nested struct gets array", "[from_json][typed][throw]") {
    auto j = json{{"name", "Bob"}, {"address", json::array({1, 2, 3})}};
    REQUIRE_THROWS_AS(from_json<Person>(j), std::logic_error);
}

TEST_CASE("typed from_json: throws on invalid JSON string", "[from_json][typed][throw]") {
    REQUIRE_THROWS(from_json<SimpleArgs>(std::string("not valid json")));
}

TEST_CASE("typed from_json: throws on type mismatch — map field gets array", "[from_json][typed][throw]") {
    auto j = json{{"settings", json::array({1, 2, 3})}};
    REQUIRE_THROWS_AS(from_json<EmptyConfig>(j), std::logic_error);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic from_json — factory
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic from_json: factory creates instance with values", "[from_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count", 100)
        .field<bool>("active", false);

    auto j = json{{"title", "Party"}, {"count", 50}, {"active", true}};
    auto obj = t.create(j);

    REQUIRE(obj.get<std::string>("title") == "Party");
    REQUIRE(obj.get<int>("count") == 50);
    REQUIRE(obj.get<bool>("active") == true);
}

TEST_CASE("dynamic from_json: factory preserves defaults for missing keys", "[from_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Untitled"))
        .field<int>("count", 100);

    auto j = json{{"title", "Custom"}};
    auto obj = t.create(j);

    REQUIRE(obj.get<std::string>("title") == "Custom");
    REQUIRE(obj.get<int>("count") == 100);
}

TEST_CASE("dynamic from_json: factory ignores extra keys", "[from_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title");

    auto j = json{{"title", "Party"}, {"unknown", 42}};
    auto obj = t.create(j);

    REQUIRE(obj.get<std::string>("title") == "Party");
}

TEST_CASE("dynamic from_json: factory with empty JSON", "[from_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Default"))
        .field<int>("count", 0);

    auto obj = t.create(json::object());

    REQUIRE(obj.get<std::string>("title") == "Default");
    REQUIRE(obj.get<int>("count") == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic load_json — in-place overlay
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic load_json: overlays values onto existing instance", "[from_json][dynamic][load_json]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count", 100);
    auto obj = t.create();

    obj.set("title", std::string("Original"));

    obj.load_json(json{{"count", 50}});

    REQUIRE(obj.get<std::string>("title") == "Original");
    REQUIRE(obj.get<int>("count") == 50);
}

TEST_CASE("dynamic load_json: overwrites previously set values", "[from_json][dynamic][load_json]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    auto obj = t.create();

    obj.set("title", std::string("First"));
    obj.load_json(json{{"title", "Second"}});

    REQUIRE(obj.get<std::string>("title") == "Second");
}

TEST_CASE("dynamic load_json: throws on non-object JSON", "[from_json][dynamic][load_json][throw]") {
    auto t = type_def("Event")
        .field<int>("x");
    auto obj = t.create();

    REQUIRE_THROWS_AS(obj.load_json(json(42)), std::logic_error);
    REQUIRE_THROWS_AS(obj.load_json(json::array()), std::logic_error);
    REQUIRE_THROWS_AS(obj.load_json(json("string")), std::logic_error);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic from_json — error cases
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic from_json: factory throws on non-object", "[from_json][dynamic][throw]") {
    auto t = type_def("Event")
        .field<int>("x");

    REQUIRE_THROWS_AS(t.create(json(42)), std::logic_error);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic from_json — type_instance preserves type() access
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic from_json: type() metadata accessible after deserialization", "[from_json][dynamic][meta]") {
    struct endpoint_info { const char* path = ""; };

    auto t = type_def("Event")
        .meta<endpoint_info>({.path = "/events"})
        .field<std::string>("title")
        .field<int>("count", 100);

    auto obj = t.create(json{{"title", "Party"}});

    REQUIRE(obj.type().name() == "Event");
    REQUIRE(obj.type().has_meta<endpoint_info>());
    REQUIRE(obj.type().field_count() == 2);
}

// ═════════════════════════════════════════════════════════════════════════
// Enum fields — both directions
// ═════════════════════════════════════════════════════════════════════════

enum class HttpMethod { GET, POST, PUT, DELETE_METHOD };

enum class Color : int { red = 0, green = 1, blue = 2 };

struct EnumStruct {
    field<HttpMethod>  method;
    field<std::string> name;
};

struct MultiEnumStruct {
    field<Color>       color;
    field<HttpMethod>  method;
    field<std::string> label;
};

// ── to_json: enums serialize as strings ──────────────────────────────────

TEST_CASE("typed to_json: enum field serializes as string", "[json][enum][to_json]") {
    EnumStruct es;
    es.method = HttpMethod::POST;
    es.name = "create-dog";

    auto j = to_json(es);

    REQUIRE(j["method"] == "POST");
    REQUIRE(j["name"] == "create-dog");
}

TEST_CASE("typed to_json: enum default serializes as string", "[json][enum][to_json]") {
    EnumStruct es;
    es.name = "default";

    auto j = to_json(es);

    REQUIRE(j["method"] == "GET");
}

TEST_CASE("typed to_json: multiple enum fields serialize as strings", "[json][enum][to_json]") {
    MultiEnumStruct ms;
    ms.color = Color::blue;
    ms.method = HttpMethod::PUT;
    ms.label = "fancy";

    auto j = to_json(ms);

    REQUIRE(j["color"] == "blue");
    REQUIRE(j["method"] == "PUT");
    REQUIRE(j["label"] == "fancy");
}

// ── from_json: enums deserialize from strings ───────────────────────────

TEST_CASE("typed from_json: enum field deserializes from string", "[json][enum][from_json]") {
    auto j = json{{"method", "PUT"}, {"name", "update-dog"}};
    auto es = from_json<EnumStruct>(j);

    REQUIRE(es.method.value == HttpMethod::PUT);
    REQUIRE(es.name.value == "update-dog");
}

TEST_CASE("typed from_json: enum field also accepts integer", "[json][enum][from_json]") {
    auto j = json{{"method", 2}, {"name", "update-dog"}};
    auto es = from_json<EnumStruct>(j);

    REQUIRE(es.method.value == HttpMethod::PUT);
}

TEST_CASE("typed from_json: enum field default when key missing", "[json][enum][from_json]") {
    auto j = json{{"name", "test"}};
    auto es = from_json<EnumStruct>(j);

    REQUIRE(es.method.value == HttpMethod::GET);
}

// ── round-trips ─────────────────────────────────────────────────────────

TEST_CASE("typed: enum round-trip", "[json][enum][roundtrip]") {
    EnumStruct original;
    original.method = HttpMethod::DELETE_METHOD;
    original.name = "remove-dog";

    auto j = to_json(original);
    REQUIRE(j["method"] == "DELETE_METHOD");

    auto restored = from_json<EnumStruct>(j);

    REQUIRE(restored.method.value == original.method.value);
    REQUIRE(restored.name.value == original.name.value);
}

TEST_CASE("typed: multiple enum fields round-trip", "[json][enum][roundtrip]") {
    MultiEnumStruct original;
    original.color = Color::blue;
    original.method = HttpMethod::POST;
    original.label = "fancy";

    auto j = to_json(original);

    REQUIRE(j["color"] == "blue");
    REQUIRE(j["method"] == "POST");
    REQUIRE(j["label"] == "fancy");

    auto restored = from_json<MultiEnumStruct>(j);

    REQUIRE(restored.color.value == Color::blue);
    REQUIRE(restored.method.value == HttpMethod::POST);
    REQUIRE(restored.label.value == "fancy");
}

// ── error cases ─────────────────────────────────────────────────────────

TEST_CASE("typed from_json: enum field throws on unknown string", "[json][enum][from_json][throw]") {
    auto j = json{{"method", "PATCH"}, {"name", "test"}};
    REQUIRE_THROWS_AS(from_json<EnumStruct>(j), std::logic_error);
}

TEST_CASE("typed from_json: enum field throws on bool value", "[json][enum][from_json][throw]") {
    auto j = json{{"method", true}, {"name", "test"}};
    REQUIRE_THROWS_AS(from_json<EnumStruct>(j), std::logic_error);
}

TEST_CASE("typed from_json: enum field throws on object value", "[json][enum][from_json][throw]") {
    auto j = json{{"method", json::object()}, {"name", "test"}};
    REQUIRE_THROWS_AS(from_json<EnumStruct>(j), std::logic_error);
}

TEST_CASE("typed from_json: enum field throws on array value", "[json][enum][from_json][throw]") {
    auto j = json{{"method", json::array()}, {"name", "test"}};
    REQUIRE_THROWS_AS(from_json<EnumStruct>(j), std::logic_error);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed structs used as "hybrid" — field<> members with to_json/from_json
// ═════════════════════════════════════════════════════════════════════════
//
// Note: hybrid plain structs (no field<> members) don't satisfy
// reflected_struct, so from_json<PlainDog> isn't available through
// the typed template. Hybrid set/get works via type_def<T>.field(&T::m, "name"),
// but JSON serialization requires field<> members. These tests verify
// that typed structs with field<> (which is also the hybrid base) work.

TEST_CASE("typed/hybrid to_json: struct with field<> serializes correctly", "[to_json][hybrid]") {
    SimpleArgs args;
    args.name = "Alice";
    args.age = 30;
    args.active = true;

    auto j = to_json(args);

    REQUIRE(j["name"] == "Alice");
    REQUIRE(j["age"] == 30);
    REQUIRE(j["active"] == true);
}

TEST_CASE("typed/hybrid to_json + from_json round-trip", "[json][hybrid][roundtrip]") {
    SimpleArgs original;
    original.name = "Bob";
    original.age = 25;
    original.active = false;

    auto j = to_json(original);
    auto restored = from_json<SimpleArgs>(j);

    REQUIRE(restored.name.value == "Bob");
    REQUIRE(restored.age.value == 25);
    REQUIRE(restored.active.value == false);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic to_json — type_instance serialization
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic to_json: basic primitive fields", "[to_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count")
        .field<bool>("active");
    auto obj = t.create();

    obj.set("title", std::string("Party"));
    obj.set("count", 42);
    obj.set("active", true);

    auto j = obj.to_json();

    REQUIRE(j["title"] == "Party");
    REQUIRE(j["count"] == 42);
    REQUIRE(j["active"] == true);
}

TEST_CASE("dynamic to_json: preserves default values", "[to_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Untitled"))
        .field<int>("count", 100);
    auto obj = t.create();

    auto j = obj.to_json();

    REQUIRE(j["title"] == "Untitled");
    REQUIRE(j["count"] == 100);
}

TEST_CASE("dynamic to_json: all primitive types", "[to_json][dynamic]") {
    auto t = type_def("Types")
        .field<std::string>("s", std::string("hello"))
        .field<bool>("b", true)
        .field<int>("i", 42)
        .field<int64_t>("i64", int64_t(9000000000LL))
        .field<uint64_t>("u64", uint64_t(18000000000ULL))
        .field<double>("d", 3.14)
        .field<float>("f", 2.5f);
    auto obj = t.create();

    auto j = obj.to_json();

    REQUIRE(j["s"] == "hello");
    REQUIRE(j["b"] == true);
    REQUIRE(j["i"] == 42);
    REQUIRE(j["i64"] == 9000000000LL);
    REQUIRE(j["u64"] == 18000000000ULL);
    REQUIRE(j["d"] == 3.14);
    REQUIRE(j["f"].get<float>() == Catch::Approx(2.5f));
}

TEST_CASE("dynamic to_json: empty type_def produces empty object", "[to_json][dynamic]") {
    auto t = type_def("Empty");
    auto obj = t.create();

    auto j = obj.to_json();

    REQUIRE(j.is_object());
    REQUIRE(j.empty());
}

TEST_CASE("dynamic to_json: fields with no defaults get default-constructed values", "[to_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count");
    auto obj = t.create();

    auto j = obj.to_json();

    REQUIRE(j["title"] == "");
    REQUIRE(j["count"] == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic to_json_string
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic to_json_string: compact output", "[to_json][dynamic][string]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Party"));
    auto obj = t.create();

    auto s = obj.to_json_string();

    auto j = json::parse(s);
    REQUIRE(j["title"] == "Party");
    REQUIRE(s.find('\n') == std::string::npos);
}

TEST_CASE("dynamic to_json_string: pretty output", "[to_json][dynamic][string]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Party"));
    auto obj = t.create();

    auto s = obj.to_json_string(2);

    auto j = json::parse(s);
    REQUIRE(j["title"] == "Party");
    REQUIRE(s.find('\n') != std::string::npos);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic round-trip: to_json → load_json
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic round-trip: to_json then load_json", "[json][dynamic][roundtrip]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count")
        .field<bool>("active");
    auto obj = t.create();

    obj.set("title", std::string("Dog Party"));
    obj.set("count", 50);
    obj.set("active", true);

    auto j = obj.to_json();

    auto obj2 = t.create();
    obj2.load_json(j);

    REQUIRE(obj2.get<std::string>("title") == "Dog Party");
    REQUIRE(obj2.get<int>("count") == 50);
    REQUIRE(obj2.get<bool>("active") == true);
}

TEST_CASE("dynamic round-trip: create(json) then to_json", "[json][dynamic][roundtrip]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count", 100);

    auto original = json{{"title", "Party"}, {"count", 50}};
    auto obj = t.create(original);
    auto roundtripped = obj.to_json();

    REQUIRE(roundtripped["title"] == "Party");
    REQUIRE(roundtripped["count"] == 50);
}

TEST_CASE("dynamic round-trip: partial load preserves untouched fields in to_json", "[json][dynamic][roundtrip]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Default"))
        .field<int>("count", 100)
        .field<bool>("active", false);
    auto obj = t.create();

    obj.load_json(json{{"title", "Custom"}});

    auto j = obj.to_json();

    REQUIRE(j["title"] == "Custom");
    REQUIRE(j["count"] == 100);
    REQUIRE(j["active"] == false);
}

// ═════════════════════════════════════════════════════════════════════════
// Dynamic to_json with metas — metas don't appear in JSON
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic to_json: metas are not serialized", "[to_json][dynamic][meta]") {
    struct endpoint_info { const char* path = ""; };

    auto t = type_def("Event")
        .meta<endpoint_info>({.path = "/events"})
        .field<std::string>("title", std::string("Party"));
    auto obj = t.create();

    auto j = obj.to_json();

    REQUIRE(j.size() == 1);
    REQUIRE(j["title"] == "Party");
    REQUIRE(!j.contains("endpoint"));
    REQUIRE(!j.contains("path"));
}

TEST_CASE("dynamic to_json: set after create reflects updated value", "[to_json][dynamic]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Original"));
    auto obj = t.create();

    obj.set("title", std::string("Updated"));

    REQUIRE(obj.to_json()["title"] == "Updated");
}

TEST_CASE("dynamic to_json_string: round-trip parse matches to_json", "[to_json][dynamic][string]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Party"))
        .field<int>("count", 42);
    auto obj = t.create();

    auto from_string = json::parse(obj.to_json_string());
    auto from_method = obj.to_json();

    REQUIRE(from_string == from_method);
}

TEST_CASE("dynamic to_json_string: empty type produces empty object string", "[to_json][dynamic][string]") {
    auto t = type_def("Empty");
    auto obj = t.create();

    REQUIRE(obj.to_json_string() == "{}");
}

// ═════════════════════════════════════════════════════════════════════════
// Hybrid to_json — structs with meta<> + field<> members
// ═════════════════════════════════════════════════════════════════════════
//
// These use the shared model types from test_model_types.hpp (Dog,
// MetaDog, MixedStruct, etc.) which have meta<> members that must
// NOT appear in JSON output.

struct hjson_endpoint_info {
    const char* path   = "";
    const char* method = "GET";
};

struct hjson_help_info {
    const char* summary = "";
};

struct HJsonDog {
    meta<hjson_endpoint_info> endpoint{{.path = "/dogs", .method = "POST"}};
    meta<hjson_help_info>     help{{.summary = "A good boy"}};

    field<std::string>  name;
    field<int>          age;
    field<std::string>  breed;
};

struct HJsonMixed {
    meta<hjson_help_info>  help{{.summary = "mixed test"}};

    field<std::string> label;
    int                counter = 0;
    field<int>         score;
};

struct HJsonMetaOnly {
    meta<hjson_endpoint_info> endpoint{{.path = "/health", .method = "GET"}};
    meta<hjson_help_info>     help{{.summary = "health check"}};
};

#ifndef COLLAB_FIELD_HAS_PFR
template <>
constexpr auto collab::model::struct_info<HJsonDog>() {
    return collab::model::field_info<HJsonDog>("endpoint", "help", "name", "age", "breed");
}

template <>
constexpr auto collab::model::struct_info<HJsonMixed>() {
    return collab::model::field_info<HJsonMixed>("help", "label", "counter", "score");
}

template <>
constexpr auto collab::model::struct_info<HJsonMetaOnly>() {
    return collab::model::field_info<HJsonMetaOnly>("endpoint", "help");
}
#endif

TEST_CASE("hybrid to_json: struct with metas — only fields serialized", "[to_json][hybrid][meta]") {
    HJsonDog d;
    d.name = "Rex";
    d.age = 3;
    d.breed = "Husky";

    auto j = to_json(d);

    REQUIRE(j.size() == 3);
    REQUIRE(j["name"] == "Rex");
    REQUIRE(j["age"] == 3);
    REQUIRE(j["breed"] == "Husky");
    REQUIRE(!j.contains("endpoint"));
    REQUIRE(!j.contains("help"));
}

TEST_CASE("hybrid to_json: mixed struct skips meta and plain members", "[to_json][hybrid][meta]") {
    HJsonMixed ms;
    ms.label = "hello";
    ms.counter = 999;
    ms.score = 42;

    auto j = to_json(ms);

    REQUIRE(j.size() == 2);
    REQUIRE(j["label"] == "hello");
    REQUIRE(j["score"] == 42);
    REQUIRE(!j.contains("counter"));
    REQUIRE(!j.contains("help"));
}

TEST_CASE("hybrid from_json: struct with metas — metas untouched", "[from_json][hybrid][meta]") {
    auto j = json{{"name", "Rex"}, {"age", 3}, {"breed", "Husky"}};
    auto d = from_json<HJsonDog>(j);

    REQUIRE(d.name.value == "Rex");
    REQUIRE(d.age.value == 3);
    REQUIRE(d.breed.value == "Husky");
    REQUIRE(std::string_view{d.endpoint->path} == "/dogs");
    REQUIRE(std::string_view{d.endpoint->method} == "POST");
    REQUIRE(std::string_view{d.help->summary} == "A good boy");
}

TEST_CASE("hybrid from_json: missing keys preserve defaults, metas untouched", "[from_json][hybrid][meta]") {
    auto j = json{{"name", "Rex"}};
    auto d = from_json<HJsonDog>(j);

    REQUIRE(d.name.value == "Rex");
    REQUIRE(d.age.value == 0);
    REQUIRE(d.breed.value.empty());
    REQUIRE(std::string_view{d.endpoint->path} == "/dogs");
}

TEST_CASE("hybrid from_json: extra keys ignored, metas untouched", "[from_json][hybrid][meta]") {
    auto j = json{{"name", "Rex"}, {"endpoint", "junk"}, {"help", 42}};
    auto d = from_json<HJsonDog>(j);

    REQUIRE(d.name.value == "Rex");
    REQUIRE(std::string_view{d.endpoint->path} == "/dogs");
}

TEST_CASE("hybrid from_json: empty JSON gives defaults, metas untouched", "[from_json][hybrid][meta]") {
    auto d = from_json<HJsonDog>(json::object());

    REQUIRE(d.name.value.empty());
    REQUIRE(d.age.value == 0);
    REQUIRE(std::string_view{d.endpoint->path} == "/dogs");
}

TEST_CASE("hybrid from_json: throws on non-object", "[from_json][hybrid][throw]") {
    REQUIRE_THROWS_AS(from_json<HJsonDog>(json(42)), std::logic_error);
    REQUIRE_THROWS_AS(from_json<HJsonDog>(json::array()), std::logic_error);
}

TEST_CASE("hybrid from_json: throws on type mismatch", "[from_json][hybrid][throw]") {
    auto j = json{{"name", 42}};
    REQUIRE_THROWS_AS(from_json<HJsonDog>(j), std::logic_error);
}

TEST_CASE("hybrid round-trip: to_json then from_json preserves fields", "[json][hybrid][roundtrip]") {
    HJsonDog original;
    original.name = "Buddy";
    original.age = 5;
    original.breed = "Golden";

    auto j = to_json(original);
    auto restored = from_json<HJsonDog>(j);

    REQUIRE(restored.name.value == "Buddy");
    REQUIRE(restored.age.value == 5);
    REQUIRE(restored.breed.value == "Golden");
    REQUIRE(std::string_view{restored.endpoint->path} == "/dogs");
}

TEST_CASE("hybrid from_json: from JSON string", "[from_json][hybrid][string]") {
    auto d = from_json<HJsonDog>(std::string(R"({"name":"Rex","age":3,"breed":"Husky"})"));

    REQUIRE(d.name.value == "Rex");
    REQUIRE(d.age.value == 3);
}

TEST_CASE("hybrid to_json_string: compact output", "[to_json][hybrid][string]") {
    HJsonDog d;
    d.name = "Rex";
    d.age = 3;
    d.breed = "Husky";

    auto s = to_json_string(d);

    auto j = json::parse(s);
    REQUIRE(j["name"] == "Rex");
    REQUIRE(s.find('\n') == std::string::npos);
}

TEST_CASE("hybrid to_json_string: pretty output", "[to_json][hybrid][string]") {
    HJsonDog d;
    d.name = "Rex";
    d.age = 3;
    d.breed = "Husky";

    auto s = to_json_string(d, 2);

    auto j = json::parse(s);
    REQUIRE(j["name"] == "Rex");
    REQUIRE(s.find('\n') != std::string::npos);
}
