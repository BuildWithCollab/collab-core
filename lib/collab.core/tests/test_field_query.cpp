#include "test_model_types.hpp"

// ═════════════════════════════════════════════════════════════════════════
// field() returns view with correct name
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: field() returns view with correct name", "[type_def][typed][field_query]") {
    type_def<SimpleArgs> t;
    auto fv = t.field("name");
    REQUIRE(fv.name() == "name");
}

TEST_CASE("hybrid: field().name()", "[type_def][hybrid][field_query]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");
    REQUIRE(t.field("name").name() == "name");
}

TEST_CASE("dynamic: field().name()", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    REQUIRE(t.field("title").name() == "title");
}

// ═════════════════════════════════════════════════════════════════════════
// field() works for each field
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: field() works for each field", "[type_def][typed][field_query]") {
    type_def<Dog> t;
    REQUIRE(t.field("name").name() == "name");
    REQUIRE(t.field("age").name() == "age");
    REQUIRE(t.field("breed").name() == "breed");
}

TEST_CASE("hybrid: field() works for each field", "[type_def][hybrid][field_query]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name")
        .field(&PlainDog::age, "age")
        .field(&PlainDog::breed, "breed");
    REQUIRE(t.field("name").name() == "name");
    REQUIRE(t.field("age").name() == "age");
    REQUIRE(t.field("breed").name() == "breed");
}

TEST_CASE("dynamic: field() works for each field", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<std::string>("title")
        .field<int>("count")
        .field<bool>("active");
    REQUIRE(t.field("title").name() == "title");
    REQUIRE(t.field("count").name() == "count");
    REQUIRE(t.field("active").name() == "active");
}

// ═════════════════════════════════════════════════════════════════════════
// field_view has_default()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic: field().has_default() false", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<std::string>("title");
    REQUIRE(t.field("title").has_default() == false);
}

TEST_CASE("dynamic: field().has_default() true", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<int>("count", 100);
    REQUIRE(t.field("count").has_default() == true);
}

// ═════════════════════════════════════════════════════════════════════════
// field_view default_value<V>()
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic: field().default_value<T>()", "[type_def][dynamic][field_query]") {
    auto t = type_def("Event")
        .field<int>("count", 100)
        .field<std::string>("title", std::string("Untitled"));
    REQUIRE(t.field("count").default_value<int>() == 100);
    REQUIRE(t.field("title").default_value<std::string>() == "Untitled");
}

// ═════════════════════════════════════════════════════════════════════════
// Edge cases
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: single field struct field access", "[type_def][typed][field_query]") {
    type_def<SingleField> t;
    REQUIRE(t.field("value").name() == "value");
}

TEST_CASE("hybrid: single field struct field access", "[type_def][hybrid][field_query]") {
    auto t = type_def<PlainPoint>()
        .field(&PlainPoint::x, "x");
    REQUIRE(t.field("x").name() == "x");
}

TEST_CASE("dynamic: single field struct field access", "[type_def][dynamic][field_query]") {
    auto t = type_def("Thing")
        .field<int>("value");
    REQUIRE(t.field("value").name() == "value");
}

TEST_CASE("typed: field() returns valid view for each Dog field", "[type_def][typed][field_query]") {
    type_def<Dog> t;
    REQUIRE(t.field("breed").name() == "breed");
}
