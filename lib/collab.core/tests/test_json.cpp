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
struct VecOfVecs;
struct MapOfVecs;
struct VecOfMaps;
struct OptionalVec;
struct VecOfOptionals;
struct DeepInner;
struct DeepMiddle;
struct DeepOuter;
struct DenseMapOfStructs;
struct PersonList;
struct VecOfEnums;
struct MapOfEnums;

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

template <>
constexpr auto collab::model::struct_info<VecOfVecs>() {
    return collab::model::field_info<VecOfVecs>("matrix");
}

template <>
constexpr auto collab::model::struct_info<MapOfVecs>() {
    return collab::model::field_info<MapOfVecs>("grouped");
}

template <>
constexpr auto collab::model::struct_info<VecOfMaps>() {
    return collab::model::field_info<VecOfMaps>("records");
}

template <>
constexpr auto collab::model::struct_info<OptionalVec>() {
    return collab::model::field_info<OptionalVec>("maybe_tags");
}

template <>
constexpr auto collab::model::struct_info<VecOfOptionals>() {
    return collab::model::field_info<VecOfOptionals>("scores");
}

template <>
constexpr auto collab::model::struct_info<DeepInner>() {
    return collab::model::field_info<DeepInner>("value");
}

template <>
constexpr auto collab::model::struct_info<DeepMiddle>() {
    return collab::model::field_info<DeepMiddle>("label", "inner");
}

template <>
constexpr auto collab::model::struct_info<DeepOuter>() {
    return collab::model::field_info<DeepOuter>("name", "middle", "items");
}

template <>
constexpr auto collab::model::struct_info<DenseMapOfStructs>() {
    return collab::model::field_info<DenseMapOfStructs>("locations");
}

template <>
constexpr auto collab::model::struct_info<PersonList>() {
    return collab::model::field_info<PersonList>("org", "people");
}

template <>
constexpr auto collab::model::struct_info<VecOfEnums>() {
    return collab::model::field_info<VecOfEnums>("methods");
}

template <>
constexpr auto collab::model::struct_info<MapOfEnums>() {
    return collab::model::field_info<MapOfEnums>("palette");
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

struct VecOfVecs {
    field<std::vector<std::vector<int>>> matrix;
};

struct MapOfVecs {
    field<std::map<std::string, std::vector<std::string>>> grouped;
};

struct VecOfMaps {
    field<std::vector<std::map<std::string, int>>> records;
};

struct OptionalVec {
    field<std::optional<std::vector<std::string>>> maybe_tags;
};

struct VecOfOptionals {
    field<std::vector<std::optional<int>>> scores;
};

struct DeepInner {
    field<std::string> value;
};

struct DeepMiddle {
    field<std::string>  label;
    field<DeepInner>    inner;
};

struct DeepOuter {
    field<std::string>              name;
    field<DeepMiddle>               middle;
    field<std::vector<DeepInner>>   items;
};

struct DenseMapOfStructs {
    field<ankerl::unordered_dense::map<std::string, Address>> locations;
};

struct PersonList {
    field<std::string>         org;
    field<std::vector<Person>> people;
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
// Typed to_json — explicit serialization tests (not just round-trips)
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed to_json: primitive fields", "[to_json][typed]") {
    SimpleArgs args;
    args.name = "Alice";
    args.age = 30;
    args.active = true;

    auto j = to_json(args);

    REQUIRE(j.is_object());
    REQUIRE(j["name"] == "Alice");
    REQUIRE(j["age"] == 30);
    REQUIRE(j["active"] == true);
}

TEST_CASE("typed to_json: default-constructed struct", "[to_json][typed]") {
    SimpleArgs args;
    auto j = to_json(args);

    REQUIRE(j["name"] == "");
    REQUIRE(j["age"] == 0);
    REQUIRE(j["active"] == false);
}

TEST_CASE("typed to_json: struct with defaults", "[to_json][typed]") {
    WithDefaults w;
    auto j = to_json(w);

    REQUIRE(j["city"] == "Portland");
    REQUIRE(j["days"] == 7);
}

TEST_CASE("typed to_json: nested struct", "[to_json][typed][nested]") {
    Person p;
    p.name = "Bob";
    p.address.value.street = "123 Main St";
    p.address.value.zip = "97201";

    auto j = to_json(p);

    REQUIRE(j["name"] == "Bob");
    REQUIRE(j["address"].is_object());
    REQUIRE(j["address"]["street"] == "123 Main St");
    REQUIRE(j["address"]["zip"] == "97201");
}

TEST_CASE("typed to_json: vector of strings", "[to_json][typed][vector]") {
    TaggedItem item;
    item.title = "Post";
    item.tags.value = {"c++", "modules", "reflection"};

    auto j = to_json(item);

    REQUIRE(j["tags"].is_array());
    REQUIRE(j["tags"].size() == 3);
    REQUIRE(j["tags"][0] == "c++");
    REQUIRE(j["tags"][1] == "modules");
    REQUIRE(j["tags"][2] == "reflection");
}

TEST_CASE("typed to_json: empty vector", "[to_json][typed][vector]") {
    TaggedItem item;
    item.title = "Empty";

    auto j = to_json(item);

    REQUIRE(j["tags"].is_array());
    REQUIRE(j["tags"].empty());
}

TEST_CASE("typed to_json: vector of structs", "[to_json][typed][vector]") {
    Team team;
    team.team_name = "Pirates";
    SimpleArgs a1; a1.name = "Alice"; a1.age = 30; a1.active = true;
    SimpleArgs a2; a2.name = "Bob"; a2.age = 25; a2.active = false;
    team.members.value = {a1, a2};

    auto j = to_json(team);

    REQUIRE(j["members"].is_array());
    REQUIRE(j["members"].size() == 2);
    REQUIRE(j["members"][0]["name"] == "Alice");
    REQUIRE(j["members"][0]["age"] == 30);
    REQUIRE(j["members"][1]["name"] == "Bob");
}

TEST_CASE("typed to_json: optional present", "[to_json][typed][optional]") {
    MaybeNickname m;
    m.name = "Alice";
    m.nickname.value = "Ali";

    auto j = to_json(m);

    REQUIRE(j["nickname"] == "Ali");
}

TEST_CASE("typed to_json: optional absent", "[to_json][typed][optional]") {
    MaybeNickname m;
    m.name = "Bob";

    auto j = to_json(m);

    REQUIRE(j["nickname"].is_null());
}

TEST_CASE("typed to_json: optional nested struct present", "[to_json][typed][optional][nested]") {
    Outer o;
    o.name = "test";
    o.extra.value = Inner{};
    o.extra.value->x = 42;

    auto j = to_json(o);

    REQUIRE(j["extra"].is_object());
    REQUIRE(j["extra"]["x"] == 42);
}

TEST_CASE("typed to_json: optional nested struct absent", "[to_json][typed][optional][nested]") {
    Outer o;
    o.name = "test";

    auto j = to_json(o);

    REQUIRE(j["extra"].is_null());
}

TEST_CASE("typed to_json: std::map<string, int>", "[to_json][typed][map]") {
    Config c;
    c.name = "app";
    c.settings.value = {{"timeout", 30}, {"retries", 3}};

    auto j = to_json(c);

    REQUIRE(j["settings"].is_object());
    REQUIRE(j["settings"]["timeout"] == 30);
    REQUIRE(j["settings"]["retries"] == 3);
}

TEST_CASE("typed to_json: map of structs", "[to_json][typed][map]") {
    ServiceMap sm;
    Endpoint api; api.url = "https://api.example.com"; api.port = 443;
    sm.services.value = {{"api", api}};

    auto j = to_json(sm);

    REQUIRE(j["services"]["api"].is_object());
    REQUIRE(j["services"]["api"]["url"] == "https://api.example.com");
    REQUIRE(j["services"]["api"]["port"] == 443);
}

TEST_CASE("typed to_json: empty map", "[to_json][typed][map]") {
    EmptyConfig ec;
    auto j = to_json(ec);

    REQUIRE(j["settings"].is_object());
    REQUIRE(j["settings"].empty());
}

TEST_CASE("typed to_json: std::set<string>", "[to_json][typed][set]") {
    TagSet ts;
    ts.tags.value = {"alpha", "beta", "gamma"};

    auto j = to_json(ts);

    REQUIRE(j["tags"].is_array());
    REQUIRE(j["tags"].size() == 3);
}

TEST_CASE("typed to_json: std::unordered_set<int>", "[to_json][typed][set]") {
    IdSet is;
    is.ids.value = {10, 20, 30};

    auto j = to_json(is);

    REQUIRE(j["ids"].is_array());
    REQUIRE(j["ids"].size() == 3);
}

TEST_CASE("typed to_json: ankerl::unordered_dense::map", "[to_json][typed][dense]") {
    DenseMapStruct dm;
    dm.scores.value = {{"alice", 100}, {"bob", 85}};

    auto j = to_json(dm);

    REQUIRE(j["scores"].is_object());
    REQUIRE(j["scores"]["alice"] == 100);
    REQUIRE(j["scores"]["bob"] == 85);
}

TEST_CASE("typed to_json: ankerl::unordered_dense::set", "[to_json][typed][dense]") {
    DenseSetStruct ds;
    ds.names.value = {"x", "y", "z"};

    auto j = to_json(ds);

    REQUIRE(j["names"].is_array());
    REQUIRE(j["names"].size() == 3);
}

TEST_CASE("typed to_json: double field", "[to_json][typed][numeric]") {
    Measurement m;
    m.unit = "celsius";
    m.value = 72.5;

    auto j = to_json(m);

    REQUIRE(j["value"] == 72.5);
}

TEST_CASE("typed to_json: float field", "[to_json][typed][numeric]") {
    FloatStruct fs;
    fs.weight = 3.14f;

    auto j = to_json(fs);

    REQUIRE(j["weight"].get<float>() == Catch::Approx(3.14f));
}

TEST_CASE("typed to_json: int64 and uint64", "[to_json][typed][numeric]") {
    BigNumbers bn;
    bn.signed_big = 9'000'000'000LL;
    bn.unsigned_big = 18'000'000'000ULL;

    auto j = to_json(bn);

    REQUIRE(j["signed_big"] == 9000000000LL);
    REQUIRE(j["unsigned_big"] == 18000000000ULL);
}

TEST_CASE("typed to_json: mixed struct skips non-field members", "[to_json][typed][mixed]") {
    MixedStruct ms;
    ms.visible = "hello";
    ms.internal_counter = 999;
    ms.score = 42;

    auto j = to_json(ms);

    REQUIRE(j.size() == 2);
    REQUIRE(j["visible"] == "hello");
    REQUIRE(j["score"] == 42);
    REQUIRE(!j.contains("internal_counter"));
}

TEST_CASE("typed to_json: extensions untouched in output", "[to_json][typed][with]") {
    CliArgs args;
    args.query = "hello";
    args.verbose = true;

    auto j = to_json(args);

    REQUIRE(j["query"] == "hello");
    REQUIRE(j["verbose"] == true);
    REQUIRE(!j.contains("posix"));
    REQUIRE(!j.contains("with"));
}

// ═════════════════════════════════════════════════════════════════════════
// Typed — nested collections (trying to break it 🔪)
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed to_json: vector of vectors", "[to_json][typed][nested_collections]") {
    VecOfVecs vv;
    vv.matrix.value = {{1, 2, 3}, {4, 5}, {6}};

    auto j = to_json(vv);

    REQUIRE(j["matrix"].is_array());
    REQUIRE(j["matrix"].size() == 3);
    REQUIRE(j["matrix"][0] == json::array({1, 2, 3}));
    REQUIRE(j["matrix"][1] == json::array({4, 5}));
    REQUIRE(j["matrix"][2] == json::array({6}));
}

TEST_CASE("typed from_json: vector of vectors", "[from_json][typed][nested_collections]") {
    auto j = json{{"matrix", {{1, 2, 3}, {4, 5}, {6}}}};
    auto vv = from_json<VecOfVecs>(j);

    REQUIRE(vv.matrix.value.size() == 3);
    REQUIRE(vv.matrix.value[0] == std::vector<int>{1, 2, 3});
    REQUIRE(vv.matrix.value[1] == std::vector<int>{4, 5});
    REQUIRE(vv.matrix.value[2] == std::vector<int>{6});
}

TEST_CASE("typed round-trip: vector of vectors", "[json][typed][nested_collections][roundtrip]") {
    VecOfVecs original;
    original.matrix.value = {{1, 2}, {3, 4, 5}, {}};

    auto restored = from_json<VecOfVecs>(to_json(original));

    REQUIRE(restored.matrix.value.size() == 3);
    REQUIRE(restored.matrix.value[0] == std::vector<int>{1, 2});
    REQUIRE(restored.matrix.value[2].empty());
}

TEST_CASE("typed to_json: map of vectors", "[to_json][typed][nested_collections]") {
    MapOfVecs mv;
    mv.grouped.value = {{"fruits", {"apple", "banana"}}, {"colors", {"red"}}};

    auto j = to_json(mv);

    REQUIRE(j["grouped"]["fruits"] == json::array({"apple", "banana"}));
    REQUIRE(j["grouped"]["colors"] == json::array({"red"}));
}

TEST_CASE("typed from_json: map of vectors", "[from_json][typed][nested_collections]") {
    auto j = json{{"grouped", {{"fruits", {"apple", "banana"}}, {"colors", {"red"}}}}};
    auto mv = from_json<MapOfVecs>(j);

    REQUIRE(mv.grouped.value.at("fruits").size() == 2);
    REQUIRE(mv.grouped.value.at("fruits")[0] == "apple");
    REQUIRE(mv.grouped.value.at("colors").size() == 1);
}

TEST_CASE("typed round-trip: map of vectors", "[json][typed][nested_collections][roundtrip]") {
    MapOfVecs original;
    original.grouped.value = {{"a", {"x", "y"}}, {"b", {}}};

    auto restored = from_json<MapOfVecs>(to_json(original));

    REQUIRE(restored.grouped.value.at("a") == std::vector<std::string>{"x", "y"});
    REQUIRE(restored.grouped.value.at("b").empty());
}

TEST_CASE("typed to_json: vector of maps", "[to_json][typed][nested_collections]") {
    VecOfMaps vm;
    vm.records.value = {{{"x", 1}, {"y", 2}}, {{"z", 3}}};

    auto j = to_json(vm);

    REQUIRE(j["records"].is_array());
    REQUIRE(j["records"].size() == 2);
    REQUIRE(j["records"][0]["x"] == 1);
    REQUIRE(j["records"][0]["y"] == 2);
    REQUIRE(j["records"][1]["z"] == 3);
}

TEST_CASE("typed from_json: vector of maps", "[from_json][typed][nested_collections]") {
    auto j = json{{"records", {{{"x", 1}, {"y", 2}}, {{"z", 3}}}}};
    auto vm = from_json<VecOfMaps>(j);

    REQUIRE(vm.records.value.size() == 2);
    REQUIRE(vm.records.value[0].at("x") == 1);
    REQUIRE(vm.records.value[1].at("z") == 3);
}

TEST_CASE("typed round-trip: vector of maps", "[json][typed][nested_collections][roundtrip]") {
    VecOfMaps original;
    original.records.value = {{{"a", 1}}, {{"b", 2}, {"c", 3}}};

    auto restored = from_json<VecOfMaps>(to_json(original));

    REQUIRE(restored.records.value.size() == 2);
    REQUIRE(restored.records.value[0].at("a") == 1);
    REQUIRE(restored.records.value[1].size() == 2);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed — optional + collection combos (🔪🔪)
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed to_json: optional vector present", "[to_json][typed][optional_collection]") {
    OptionalVec ov;
    ov.maybe_tags.value = std::vector<std::string>{"a", "b"};

    auto j = to_json(ov);

    REQUIRE(j["maybe_tags"].is_array());
    REQUIRE(j["maybe_tags"].size() == 2);
    REQUIRE(j["maybe_tags"][0] == "a");
}

TEST_CASE("typed to_json: optional vector absent", "[to_json][typed][optional_collection]") {
    OptionalVec ov;

    auto j = to_json(ov);

    REQUIRE(j["maybe_tags"].is_null());
}

TEST_CASE("typed from_json: optional vector present", "[from_json][typed][optional_collection]") {
    auto j = json{{"maybe_tags", {"a", "b", "c"}}};
    auto ov = from_json<OptionalVec>(j);

    REQUIRE(ov.maybe_tags.value.has_value());
    REQUIRE(ov.maybe_tags.value->size() == 3);
    REQUIRE((*ov.maybe_tags.value)[0] == "a");
}

TEST_CASE("typed from_json: optional vector null", "[from_json][typed][optional_collection]") {
    auto j = json{{"maybe_tags", nullptr}};
    auto ov = from_json<OptionalVec>(j);

    REQUIRE(!ov.maybe_tags.value.has_value());
}

TEST_CASE("typed from_json: optional vector missing key", "[from_json][typed][optional_collection]") {
    auto j = json::object();
    auto ov = from_json<OptionalVec>(j);

    REQUIRE(!ov.maybe_tags.value.has_value());
}

TEST_CASE("typed round-trip: optional vector present", "[json][typed][optional_collection][roundtrip]") {
    OptionalVec original;
    original.maybe_tags.value = std::vector<std::string>{"x", "y"};

    auto restored = from_json<OptionalVec>(to_json(original));

    REQUIRE(restored.maybe_tags.value.has_value());
    REQUIRE(*restored.maybe_tags.value == std::vector<std::string>{"x", "y"});
}

TEST_CASE("typed round-trip: optional vector absent", "[json][typed][optional_collection][roundtrip]") {
    OptionalVec original;

    auto restored = from_json<OptionalVec>(to_json(original));

    REQUIRE(!restored.maybe_tags.value.has_value());
}

TEST_CASE("typed to_json: vector of optionals", "[to_json][typed][optional_collection]") {
    VecOfOptionals vo;
    vo.scores.value = {1, std::nullopt, 3, std::nullopt, 5};

    auto j = to_json(vo);

    REQUIRE(j["scores"].is_array());
    REQUIRE(j["scores"].size() == 5);
    REQUIRE(j["scores"][0] == 1);
    REQUIRE(j["scores"][1].is_null());
    REQUIRE(j["scores"][2] == 3);
    REQUIRE(j["scores"][3].is_null());
    REQUIRE(j["scores"][4] == 5);
}

TEST_CASE("typed from_json: vector of optionals with nulls", "[from_json][typed][optional_collection]") {
    auto j = json{{"scores", {1, nullptr, 3, nullptr, 5}}};
    auto vo = from_json<VecOfOptionals>(j);

    REQUIRE(vo.scores.value.size() == 5);
    REQUIRE(vo.scores.value[0].has_value());
    REQUIRE(*vo.scores.value[0] == 1);
    REQUIRE(!vo.scores.value[1].has_value());
    REQUIRE(*vo.scores.value[2] == 3);
    REQUIRE(!vo.scores.value[3].has_value());
    REQUIRE(*vo.scores.value[4] == 5);
}

TEST_CASE("typed round-trip: vector of optionals", "[json][typed][optional_collection][roundtrip]") {
    VecOfOptionals original;
    original.scores.value = {10, std::nullopt, 30};

    auto restored = from_json<VecOfOptionals>(to_json(original));

    REQUIRE(restored.scores.value.size() == 3);
    REQUIRE(*restored.scores.value[0] == 10);
    REQUIRE(!restored.scores.value[1].has_value());
    REQUIRE(*restored.scores.value[2] == 30);
}

// ═════════════════════════════════════════════════════════════════════════
// Typed — deep nesting (3 levels + collections) (🔪🔪🔪)
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed to_json: 3-level deep nested struct", "[to_json][typed][deep_nesting]") {
    DeepOuter obj;
    obj.name = "root";
    obj.middle.value.label = "mid";
    obj.middle.value.inner.value.value = "leaf";
    DeepInner item_a; item_a.value = "a";
    DeepInner item_b; item_b.value = "b";
    obj.items.value = {item_a, item_b};

    auto j = to_json(obj);

    REQUIRE(j["name"] == "root");
    REQUIRE(j["middle"]["label"] == "mid");
    REQUIRE(j["middle"]["inner"]["value"] == "leaf");
    REQUIRE(j["items"].is_array());
    REQUIRE(j["items"].size() == 2);
    REQUIRE(j["items"][0]["value"] == "a");
    REQUIRE(j["items"][1]["value"] == "b");
}

TEST_CASE("typed from_json: 3-level deep nested struct", "[from_json][typed][deep_nesting]") {
    auto j = json{
        {"name", "root"},
        {"middle", {
            {"label", "mid"},
            {"inner", {{"value", "leaf"}}}
        }},
        {"items", {
            {{"value", "a"}},
            {{"value", "b"}},
            {{"value", "c"}}
        }}
    };
    auto obj = from_json<DeepOuter>(j);

    REQUIRE(obj.name.value == "root");
    REQUIRE(obj.middle.value.label.value == "mid");
    REQUIRE(obj.middle.value.inner.value.value.value == "leaf");
    REQUIRE(obj.items.value.size() == 3);
    REQUIRE(obj.items.value[0].value.value == "a");
    REQUIRE(obj.items.value[2].value.value == "c");
}

TEST_CASE("typed round-trip: 3-level deep nested struct", "[json][typed][deep_nesting][roundtrip]") {
    DeepOuter original;
    original.name = "root";
    original.middle.value.label = "mid";
    original.middle.value.inner.value.value = "leaf";
    DeepInner ix; ix.value = "x";
    DeepInner iy; iy.value = "y";
    original.items.value = {ix, iy};

    auto restored = from_json<DeepOuter>(to_json(original));

    REQUIRE(restored.name.value == "root");
    REQUIRE(restored.middle.value.label.value == "mid");
    REQUIRE(restored.middle.value.inner.value.value.value == "leaf");
    REQUIRE(restored.items.value.size() == 2);
    REQUIRE(restored.items.value[0].value.value == "x");
}

TEST_CASE("typed from_json: deep nested with missing inner keys", "[from_json][typed][deep_nesting]") {
    auto j = json{{"name", "root"}, {"middle", {{"label", "mid"}}}};
    auto obj = from_json<DeepOuter>(j);

    REQUIRE(obj.name.value == "root");
    REQUIRE(obj.middle.value.label.value == "mid");
    REQUIRE(obj.middle.value.inner.value.value.value.empty());
    REQUIRE(obj.items.value.empty());
}

TEST_CASE("typed to_json: vector of persons with nested addresses", "[to_json][typed][deep_nesting]") {
    PersonList pl;
    pl.org = "Collab";
    Person p1; p1.name = "Alice"; p1.address.value.street = "1st St"; p1.address.value.zip = "10001";
    Person p2; p2.name = "Bob"; p2.address.value.street = "2nd Ave"; p2.address.value.zip = "20002";
    pl.people.value = {p1, p2};

    auto j = to_json(pl);

    REQUIRE(j["org"] == "Collab");
    REQUIRE(j["people"].size() == 2);
    REQUIRE(j["people"][0]["name"] == "Alice");
    REQUIRE(j["people"][0]["address"]["street"] == "1st St");
    REQUIRE(j["people"][1]["address"]["zip"] == "20002");
}

TEST_CASE("typed from_json: vector of persons with nested addresses", "[from_json][typed][deep_nesting]") {
    auto j = json{
        {"org", "Collab"},
        {"people", {
            {{"name", "Alice"}, {"address", {{"street", "1st St"}, {"zip", "10001"}}}},
            {{"name", "Bob"}, {"address", {{"street", "2nd Ave"}, {"zip", "20002"}}}}
        }}
    };
    auto pl = from_json<PersonList>(j);

    REQUIRE(pl.org.value == "Collab");
    REQUIRE(pl.people.value.size() == 2);
    REQUIRE(pl.people.value[0].name.value == "Alice");
    REQUIRE(pl.people.value[0].address.value.street.value == "1st St");
    REQUIRE(pl.people.value[1].address.value.zip.value == "20002");
}

TEST_CASE("typed round-trip: vector of persons with nested addresses", "[json][typed][deep_nesting][roundtrip]") {
    PersonList original;
    original.org = "Pirates";
    Person p; p.name = "Rex"; p.address.value.street = "Dog Ln"; p.address.value.zip = "99999";
    original.people.value = {p};

    auto restored = from_json<PersonList>(to_json(original));

    REQUIRE(restored.people.value.size() == 1);
    REQUIRE(restored.people.value[0].address.value.street.value == "Dog Ln");
}

// ═════════════════════════════════════════════════════════════════════════
// Typed — dense containers with struct values
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed to_json: ankerl dense map of structs", "[to_json][typed][dense]") {
    DenseMapOfStructs dm;
    Address home; home.street = "123 Main"; home.zip = "97201";
    Address work; work.street = "456 Corp Dr"; work.zip = "97202";
    dm.locations.value = {{"home", home}, {"work", work}};

    auto j = to_json(dm);

    REQUIRE(j["locations"]["home"]["street"] == "123 Main");
    REQUIRE(j["locations"]["work"]["zip"] == "97202");
}

TEST_CASE("typed from_json: ankerl dense map of structs", "[from_json][typed][dense]") {
    auto j = json{{"locations", {
        {"home", {{"street", "123 Main"}, {"zip", "97201"}}},
        {"work", {{"street", "456 Corp"}, {"zip", "97202"}}}
    }}};
    auto dm = from_json<DenseMapOfStructs>(j);

    REQUIRE(dm.locations.value.at("home").street.value == "123 Main");
    REQUIRE(dm.locations.value.at("work").zip.value == "97202");
}

TEST_CASE("typed round-trip: ankerl dense map of structs", "[json][typed][dense][roundtrip]") {
    DenseMapOfStructs original;
    Address a; a.street = "Dog St"; a.zip = "00000";
    original.locations.value = {{"park", a}};

    auto restored = from_json<DenseMapOfStructs>(to_json(original));

    REQUIRE(restored.locations.value.at("park").street.value == "Dog St");
}

// ═════════════════════════════════════════════════════════════════════════
// Typed — set edge cases
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed from_json: set deduplicates array with duplicates", "[from_json][typed][set][edge]") {
    auto j = json{{"tags", {"alpha", "beta", "alpha", "gamma", "beta"}}};
    auto ts = from_json<TagSet>(j);

    REQUIRE(ts.tags.value.size() == 3);
    REQUIRE(ts.tags.value.count("alpha") == 1);
    REQUIRE(ts.tags.value.count("beta") == 1);
    REQUIRE(ts.tags.value.count("gamma") == 1);
}

TEST_CASE("typed from_json: unordered_set deduplicates", "[from_json][typed][set][edge]") {
    auto j = json{{"ids", {1, 2, 3, 1, 2}}};
    auto is = from_json<IdSet>(j);

    REQUIRE(is.ids.value.size() == 3);
}

TEST_CASE("typed from_json: dense set deduplicates", "[from_json][typed][dense][edge]") {
    auto j = json{{"names", {"x", "y", "x", "z", "y"}}};
    auto ds = from_json<DenseSetStruct>(j);

    REQUIRE(ds.names.value.size() == 3);
}

TEST_CASE("typed round-trip: set preserves unique elements", "[json][typed][set][roundtrip]") {
    TagSet original;
    original.tags.value = {"a", "b", "c"};

    auto j = to_json(original);
    REQUIRE(j["tags"].size() == 3);

    auto restored = from_json<TagSet>(j);
    REQUIRE(restored.tags.value.size() == 3);
    REQUIRE(restored.tags.value.count("a") == 1);
    REQUIRE(restored.tags.value.count("b") == 1);
    REQUIRE(restored.tags.value.count("c") == 1);
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

struct VecOfEnums {
    field<std::vector<HttpMethod>> methods;
};

struct MapOfEnums {
    field<std::map<std::string, Color>> palette;
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
// Typed — enums inside collections
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed to_json: vector of enums", "[to_json][typed][enum][collection]") {
    VecOfEnums ve;
    ve.methods.value = {HttpMethod::GET, HttpMethod::POST, HttpMethod::PUT};

    auto j = to_json(ve);

    REQUIRE(j["methods"].is_array());
    REQUIRE(j["methods"].size() == 3);
    REQUIRE(j["methods"][0] == "GET");
    REQUIRE(j["methods"][1] == "POST");
    REQUIRE(j["methods"][2] == "PUT");
}

TEST_CASE("typed from_json: vector of enums", "[from_json][typed][enum][collection]") {
    auto j = json{{"methods", {"GET", "POST", "DELETE_METHOD"}}};
    auto ve = from_json<VecOfEnums>(j);

    REQUIRE(ve.methods.value.size() == 3);
    REQUIRE(ve.methods.value[0] == HttpMethod::GET);
    REQUIRE(ve.methods.value[1] == HttpMethod::POST);
    REQUIRE(ve.methods.value[2] == HttpMethod::DELETE_METHOD);
}

TEST_CASE("typed from_json: vector of enums with integer values", "[from_json][typed][enum][collection]") {
    auto j = json{{"methods", {0, 1, 2}}};
    auto ve = from_json<VecOfEnums>(j);

    REQUIRE(ve.methods.value.size() == 3);
    REQUIRE(ve.methods.value[0] == HttpMethod::GET);
    REQUIRE(ve.methods.value[1] == HttpMethod::POST);
    REQUIRE(ve.methods.value[2] == HttpMethod::PUT);
}

TEST_CASE("typed round-trip: vector of enums", "[json][typed][enum][collection][roundtrip]") {
    VecOfEnums original;
    original.methods.value = {HttpMethod::DELETE_METHOD, HttpMethod::GET};

    auto restored = from_json<VecOfEnums>(to_json(original));

    REQUIRE(restored.methods.value.size() == 2);
    REQUIRE(restored.methods.value[0] == HttpMethod::DELETE_METHOD);
    REQUIRE(restored.methods.value[1] == HttpMethod::GET);
}

TEST_CASE("typed to_json: map of enums", "[to_json][typed][enum][collection]") {
    MapOfEnums me;
    me.palette.value = {{"sky", Color::blue}, {"grass", Color::green}};

    auto j = to_json(me);

    REQUIRE(j["palette"]["sky"] == "blue");
    REQUIRE(j["palette"]["grass"] == "green");
}

TEST_CASE("typed from_json: map of enums", "[from_json][typed][enum][collection]") {
    auto j = json{{"palette", {{"sky", "blue"}, {"fire", "red"}}}};
    auto me = from_json<MapOfEnums>(j);

    REQUIRE(me.palette.value.at("sky") == Color::blue);
    REQUIRE(me.palette.value.at("fire") == Color::red);
}

TEST_CASE("typed round-trip: map of enums", "[json][typed][enum][collection][roundtrip]") {
    MapOfEnums original;
    original.palette.value = {{"a", Color::red}, {"b", Color::green}, {"c", Color::blue}};

    auto restored = from_json<MapOfEnums>(to_json(original));

    REQUIRE(restored.palette.value.size() == 3);
    REQUIRE(restored.palette.value.at("a") == Color::red);
    REQUIRE(restored.palette.value.at("b") == Color::green);
    REQUIRE(restored.palette.value.at("c") == Color::blue);
}

TEST_CASE("typed from_json: vector of enums throws on unknown string", "[from_json][typed][enum][collection][throw]") {
    auto j = json{{"methods", {"GET", "PATCH"}}};
    REQUIRE_THROWS_AS(from_json<VecOfEnums>(j), std::logic_error);
}

TEST_CASE("typed from_json: empty vector of enums", "[from_json][typed][enum][collection]") {
    auto j = json{{"methods", json::array()}};
    auto ve = from_json<VecOfEnums>(j);

    REQUIRE(ve.methods.value.empty());
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
