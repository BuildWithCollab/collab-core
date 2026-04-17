#include "test_model_types.hpp"

// ═══════════════════════════════════════════════════════════════════════════
// set() throws for unknown field
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: set() throws for unknown field name", "[type_def][typed][set][throw]") {
    SimpleArgs args;

    try {
        type_def<SimpleArgs>{}.set(args, "nonexistent", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nonexistent") != std::string::npos);
        REQUIRE(msg.find("SimpleArgs") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

TEST_CASE("hybrid: set() throws for unknown field", "[type_def][hybrid][set][throw]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;

    try {
        t.set(rex, "nope", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nope") != std::string::npos);
        REQUIRE(msg.find("PlainDog") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

TEST_CASE("object: set() throws for unknown field", "[object][set][throw]") {
    auto t = type_def("Event")
        .field<int>("count");
    auto obj = t.create();

    try {
        obj.set("nope", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nope") != std::string::npos);
        REQUIRE(msg.find("Event") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// set() throws for type mismatch
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: set() throws for type mismatch", "[type_def][typed][set][throw]") {
    SimpleArgs args;

    try {
        type_def<SimpleArgs>{}.set(args, "name", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("name") != std::string::npos);
        REQUIRE(msg.find("SimpleArgs") != std::string::npos);
        REQUIRE(msg.find("type mismatch") != std::string::npos);
    }
}

TEST_CASE("hybrid: set() throws for type mismatch", "[type_def][hybrid][set][throw]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;

    try {
        t.set(rex, "name", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("name") != std::string::npos);
        REQUIRE(msg.find("PlainDog") != std::string::npos);
        REQUIRE(msg.find("type mismatch") != std::string::npos);
    }
}

TEST_CASE("object: set() throws for type mismatch", "[object][set][throw]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    auto obj = t.create();

    try {
        obj.set("title", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("title") != std::string::npos);
        REQUIRE(msg.find("Event") != std::string::npos);
        REQUIRE(msg.find("type mismatch") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// set() does not modify field on type mismatch
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: set() does not modify field on type mismatch", "[type_def][typed][set][throw]") {
    SimpleArgs args;
    args.name = "Original";

    try { type_def<SimpleArgs>{}.set(args, "name", 42); } catch (const std::logic_error&) {}
    REQUIRE(args.name.value == "Original");
}

TEST_CASE("hybrid: set() does not modify field on type mismatch", "[type_def][hybrid][set][throw]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;
    rex.name = "Untouched";

    try { t.set(rex, "name", 42); } catch (const std::logic_error&) {}
    REQUIRE(rex.name == "Untouched");
}

TEST_CASE("object: set() does not modify field on type mismatch", "[object][set][throw]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string("Original"));
    auto obj = t.create();

    try { obj.set("title", 42); } catch (const std::logic_error&) {}
    REQUIRE(obj.get<std::string>("title") == "Original");
}

// ═══════════════════════════════════════════════════════════════════════════
// set() throws for meta member names
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: set() throws for meta member names", "[type_def][typed][set][throw]") {
    Dog rex;

    try {
        type_def<Dog>{}.set(rex, "endpoint", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("endpoint") != std::string::npos);
        REQUIRE(msg.find("Dog") != std::string::npos);
    }

    try {
        type_def<Dog>{}.set(rex, "help", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("help") != std::string::npos);
        REQUIRE(msg.find("Dog") != std::string::npos);
    }
}

TEST_CASE("hybrid: set() throws for meta member names", "[type_def][hybrid][set][throw]") {
    type_def<MetaDog> t;
    MetaDog rex;

    try {
        t.set(rex, "help", 42);
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("help") != std::string::npos);
        REQUIRE(msg.find("MetaDog") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// get() callback throws for unknown field
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: get() throws for unknown field name", "[type_def][typed][get][throw]") {
    SimpleArgs args;

    try {
        type_def<SimpleArgs>{}.get(args, "nonexistent", [](std::string_view, auto&) {});
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nonexistent") != std::string::npos);
        REQUIRE(msg.find("SimpleArgs") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

TEST_CASE("hybrid: get() callback throws for unknown field", "[type_def][hybrid][get][throw]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex{"Rex", 3, "Husky"};

    try {
        t.get(rex, "nope", [](std::string_view, auto&) {});
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nope") != std::string::npos);
        REQUIRE(msg.find("PlainDog") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// get() callback throws for meta member names
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: get() throws for meta member names", "[type_def][typed][get][throw]") {
    Dog rex;

    try {
        type_def<Dog>{}.get(rex, "endpoint", [](std::string_view, auto&) {});
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("endpoint") != std::string::npos);
        REQUIRE(msg.find("Dog") != std::string::npos);
    }

    try {
        type_def<Dog>{}.get(rex, "help", [](std::string_view, auto&) {});
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("help") != std::string::npos);
        REQUIRE(msg.find("Dog") != std::string::npos);
    }
}

TEST_CASE("hybrid: get() callback throws for meta member names", "[type_def][hybrid][get][throw]") {
    type_def<MetaDog> t;
    MetaDog rex;

    try {
        t.get(rex, "help", [](std::string_view, auto&) {});
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("help") != std::string::npos);
        REQUIRE(msg.find("MetaDog") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// get<V>() throws for unknown field
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: get<V>() throws for unknown field", "[type_def][typed][get_typed][throw]") {
    SimpleArgs args;
    type_def<SimpleArgs> t;

    try {
        t.get<int>(args, "nope");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nope") != std::string::npos);
        REQUIRE(msg.find("SimpleArgs") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

TEST_CASE("hybrid: get<V>() throws for unknown field", "[type_def][hybrid][get_typed][throw]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    PlainDog rex;

    try {
        t.get<std::string>(rex, "nope");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nope") != std::string::npos);
        REQUIRE(msg.find("PlainDog") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

TEST_CASE("object: get<V>() throws for unknown field", "[object][get][throw]") {
    auto t = type_def("Event")
        .field<int>("count");
    auto obj = t.create();

    try {
        obj.get<int>("nope");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nope") != std::string::npos);
        REQUIRE(msg.find("Event") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// get<V>() throws for type mismatch
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: get<V>() throws for type mismatch", "[type_def][typed][get_typed][throw]") {
    SimpleArgs args;
    args.age = 42;
    type_def<SimpleArgs> t;

    try {
        t.get<std::string>(args, "age");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("age") != std::string::npos);
        REQUIRE(msg.find("SimpleArgs") != std::string::npos);
        REQUIRE(msg.find("type mismatch") != std::string::npos);
    }
}

TEST_CASE("hybrid: get<V>() throws for type mismatch", "[type_def][hybrid][get_typed][throw]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::age, "age");
    PlainDog rex;
    rex.age = 5;

    try {
        t.get<std::string>(rex, "age");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("age") != std::string::npos);
        REQUIRE(msg.find("PlainDog") != std::string::npos);
        REQUIRE(msg.find("type mismatch") != std::string::npos);
    }
}

TEST_CASE("object: get<V>() throws for type mismatch", "[object][get][throw]") {
    auto t = type_def("Event")
        .field<int>("count", 42);
    auto obj = t.create();

    try {
        obj.get<std::string>("count");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("count") != std::string::npos);
        REQUIRE(msg.find("Event") != std::string::npos);
        REQUIRE(msg.find("type mismatch") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// field() throws for unknown name
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: field() throws for unknown name", "[type_def][typed][field_query][throw]") {
    type_def<Dog> t;

    try {
        t.field("nonexistent");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nonexistent") != std::string::npos);
        REQUIRE(msg.find("Dog") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

TEST_CASE("hybrid: field() throws for unknown name", "[type_def][hybrid][field_query][throw]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");

    try {
        t.field("nonexistent");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nonexistent") != std::string::npos);
        REQUIRE(msg.find("PlainDog") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

TEST_CASE("dynamic: field() throws for unknown name", "[type_def][dynamic][field_query][throw]") {
    auto t = type_def("Event")
        .field<std::string>("title");

    try {
        t.field("nonexistent");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("nonexistent") != std::string::npos);
        REQUIRE(msg.find("Event") != std::string::npos);
        REQUIRE(msg.find("no field") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// field() throws on empty type_def
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic: field() throws on empty type_def", "[type_def][dynamic][field_query][throw]") {
    auto t = type_def("Empty");

    try {
        t.field("anything");
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("anything") != std::string::npos);
        REQUIRE(msg.find("Empty") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// meta() throws for absent meta
// ═══════════════════════════════════════════════════════════════════════════

// typed/hybrid meta<M>() for absent types is compile-time — returns default-constructed M,
// does not throw. Only the dynamic path throws at runtime.

TEST_CASE("dynamic: meta() throws for absent meta", "[type_def][dynamic][meta][throw]") {
    auto t = type_def("Event")
        .field<int>("x");

    try {
        t.meta<endpoint_info>();
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("Event") != std::string::npos);
        REQUIRE(msg.find("no meta") != std::string::npos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// field_view throws
// ═══════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic: field_view default_value throws for wrong type", "[type_def][dynamic][field_query][throw]") {
    auto t = type_def("Event")
        .field<int>("count", 100);

    try {
        t.field("count").default_value<std::string>();
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("count") != std::string::npos);
        REQUIRE(msg.find("default_value") != std::string::npos);
        REQUIRE(msg.find("type mismatch") != std::string::npos);
    }
}

TEST_CASE("dynamic: field_view meta() throws for absent meta", "[type_def][dynamic][field_meta][throw]") {
    auto t = type_def("Event")
        .field<std::string>("title");

    try {
        t.field("title").meta<cli_meta>();
        FAIL("Expected std::logic_error");
    } catch (const std::logic_error& e) {
        std::string msg = e.what();
        REQUIRE(msg.find("title") != std::string::npos);
        REQUIRE(msg.find("no meta") != std::string::npos);
    }
}
