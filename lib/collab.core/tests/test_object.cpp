#include <catch2/catch_test_macros.hpp>

#include <any>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

import collab.core;

using namespace collab::model;

// ═════════════════════════════════════════════════════════════════════════
// Test metadata types
// ═════════════════════════════════════════════════════════════════════════

struct endpoint_info_o {
    const char* path   = "";
    const char* method = "GET";
};

struct help_info_o {
    const char* summary = "";
};

// ═════════════════════════════════════════════════════════════════════════
// Tests: object construction
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object construction from type_def", "[object]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count", 100);
    object obj(t);
    REQUIRE(obj.type().name() == "Event");
}

TEST_CASE("object constructed via create()", "[object]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count", 100);
    auto obj = t.create();
    REQUIRE(obj.type().name() == "Event");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: object defaults
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object has default values from type_def", "[object][defaults]") {
    auto t = type_def("Event")
        .field<int>("count", 100)
        .field<std::string>("title", std::string("Untitled"));
    auto obj = t.create();
    REQUIRE(obj.get<int>("count") == 100);
    REQUIRE(obj.get<std::string>("title") == "Untitled");
}

TEST_CASE("object fields without defaults get default-constructed values", "[object][defaults]") {
    auto t = type_def("Event")
        .field<int>("count")
        .field<std::string>("title");
    auto obj = t.create();
    REQUIRE(obj.get<int>("count") == 0);
    REQUIRE(obj.get<std::string>("title") == "");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: object::set()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object set() with matching type", "[object][set]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count");
    auto obj = t.create();

    REQUIRE(obj.set("title", std::string("Dog Party")));
    REQUIRE(obj.set("count", 50));
}

TEST_CASE("object set() returns false for unknown field", "[object][set]") {
    auto t = type_def("Event")
        .field<int>("count");
    auto obj = t.create();

    REQUIRE(!obj.set("nope", 42));
}

TEST_CASE("object set() returns false for type mismatch", "[object][set]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    auto obj = t.create();

    REQUIRE(!obj.set("title", 42));
}

TEST_CASE("object set() does not modify field on type mismatch", "[object][set]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Original"));
    auto obj = t.create();

    obj.set("title", 42);
    REQUIRE(obj.get<std::string>("title") == "Original");
}

TEST_CASE("object set() with const char* to string field", "[object][set]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    auto obj = t.create();

    REQUIRE(obj.set("title", "hello"));
    REQUIRE(obj.get<std::string>("title") == "hello");
}

TEST_CASE("object set() overwrites existing values", "[object][set]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("First"));
    auto obj = t.create();

    obj.set("title", std::string("Second"));
    REQUIRE(obj.get<std::string>("title") == "Second");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: object::get()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object get() returns value", "[object][get]") {
    auto t = type_def("Event")
        .field<int>("count", 42);
    auto obj = t.create();

    auto val = obj.get<int>("count");
    REQUIRE(val.has_value());
    REQUIRE(val.value() == 42);
}

TEST_CASE("object get() returns nullopt for unknown field", "[object][get]") {
    auto t = type_def("Event")
        .field<int>("count");
    auto obj = t.create();

    REQUIRE(!obj.get<int>("nope").has_value());
}

TEST_CASE("object get() returns nullopt for type mismatch", "[object][get]") {
    auto t = type_def("Event")
        .field<int>("count", 42);
    auto obj = t.create();

    REQUIRE(!obj.get<std::string>("count").has_value());
}

TEST_CASE("object get() round-trips with set()", "[object][get][set]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count")
        .field<bool>("active");
    auto obj = t.create();

    obj.set("title", std::string("Party"));
    obj.set("count", 99);
    obj.set("active", true);

    REQUIRE(obj.get<std::string>("title") == "Party");
    REQUIRE(obj.get<int>("count") == 99);
    REQUIRE(obj.get<bool>("active") == true);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: object::has()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object has() for existing field", "[object][has]") {
    auto t = type_def("Event")
        .field<int>("count");
    auto obj = t.create();

    REQUIRE(obj.has("count"));
}

TEST_CASE("object has() for missing field", "[object][has]") {
    auto t = type_def("Event")
        .field<int>("count");
    auto obj = t.create();

    REQUIRE(!obj.has("nope"));
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: object::type()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object type() returns the backing type_def", "[object][type]") {
    auto t = type_def("Event")
        .meta<endpoint_info_o>({.path = "/events"})
        .field<std::string>("title")
        .field<int>("count");
    auto obj = t.create();

    REQUIRE(obj.type().name() == "Event");
    REQUIRE(obj.type().field_count() == 2);
    REQUIRE(obj.type().has_meta<endpoint_info_o>());
    REQUIRE(std::string_view{obj.type().meta<endpoint_info_o>().path} == "/events");
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: object::for_each()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object for_each() iterates all fields", "[object][for_each]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count", 100);
    auto obj = t.create();
    obj.set("title", std::string("Party"));

    std::vector<std::string> names;
    obj.for_each([&](std::string_view name, const std::any& value) {
        names.emplace_back(name);
    });

    REQUIRE(names.size() == 2);
    REQUIRE(names[0] == "title");
    REQUIRE(names[1] == "count");
}

TEST_CASE("object for_each() provides access to values via any_cast", "[object][for_each]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count");
    auto obj = t.create();
    obj.set("title", std::string("Party"));
    obj.set("count", 42);

    std::string found_title;
    int found_count = 0;
    obj.for_each([&](std::string_view name, const std::any& value) {
        if (name == "title")
            found_title = std::any_cast<std::string>(value);
        if (name == "count")
            found_count = std::any_cast<int>(value);
    });

    REQUIRE(found_title == "Party");
    REQUIRE(found_count == 42);
}

TEST_CASE("object for_each() on empty type_def", "[object][for_each]") {
    auto t = type_def("Empty");
    auto obj = t.create();

    int count = 0;
    obj.for_each([&](std::string_view, const std::any&) { ++count; });
    REQUIRE(count == 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Multiple objects from same type_def
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("multiple objects from same type_def are independent", "[object][independence]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Default"))
        .field<int>("count", 0);

    auto a = t.create();
    auto b = t.create();

    a.set("title", std::string("Party A"));
    a.set("count", 10);

    b.set("title", std::string("Party B"));
    b.set("count", 20);

    REQUIRE(a.get<std::string>("title") == "Party A");
    REQUIRE(a.get<int>("count") == 10);
    REQUIRE(b.get<std::string>("title") == "Party B");
    REQUIRE(b.get<int>("count") == 20);
}

// ═════════════════════════════════════════════════════════════════════════
// Tests: Full integration
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("object full integration", "[object][integration]") {
    auto event_t = type_def("Event")
        .meta<endpoint_info_o>({.path = "/events"})
        .meta<help_info_o>({.summary = "An event"})
        .field<std::string>("title")
        .field<int>("attendees", 100)
        .field<bool>("verbose", false);

    // Create + defaults
    auto party = event_t.create();
    REQUIRE(party.get<int>("attendees") == 100);
    REQUIRE(party.get<bool>("verbose") == false);
    REQUIRE(party.get<std::string>("title") == "");

    // Set values
    REQUIRE(party.set("title", std::string("Dog Party")));
    REQUIRE(party.set("attendees", 50));
    REQUIRE(party.set("verbose", true));

    // Get values back
    REQUIRE(party.get<std::string>("title") == "Dog Party");
    REQUIRE(party.get<int>("attendees") == 50);
    REQUIRE(party.get<bool>("verbose") == true);

    // Type mismatch
    REQUIRE(!party.set("title", 42));
    REQUIRE(!party.get<int>("title").has_value());

    // Unknown field
    REQUIRE(!party.set("nope", 1));
    REQUIRE(!party.get<int>("nope").has_value());
    REQUIRE(!party.has("nope"));

    // Type access
    REQUIRE(party.type().name() == "Event");
    REQUIRE(party.type().has_meta<endpoint_info_o>());
    REQUIRE(party.type().field("attendees").has_default());
    REQUIRE(party.type().field("attendees").default_value<int>() == 100);
}
