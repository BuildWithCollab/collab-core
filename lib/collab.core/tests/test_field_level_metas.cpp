#include "test_model_types.hpp"

// ═════════════════════════════════════════════════════════════════════════
// Field with single meta
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: for_each_field detects field-level metas", "[type_def][typed][field_meta]") {
    bool found_verbose_cli = false;
    bool found_limit_cli = false;
    bool found_limit_render = false;

    type_def<CliArgs>{}.for_each_field([&](auto descriptor) {
        if (descriptor.name() == "verbose") {
            found_verbose_cli = descriptor.template has_meta<cli_meta>();
        }
        if (descriptor.name() == "limit") {
            found_limit_cli = descriptor.template has_meta<cli_meta>();
            found_limit_render = descriptor.template has_meta<render_meta>();
        }
    });

    REQUIRE(found_verbose_cli);
    REQUIRE(found_limit_cli);
    REQUIRE(found_limit_render);
}

TEST_CASE("hybrid: field with meta", "[type_def][hybrid][field_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name",
            with<help_info>({.summary = "Dog's name"}));

    REQUIRE(t.field("name").has_meta<help_info>());
    REQUIRE(std::string_view{t.field("name").meta<help_info>().summary} == "Dog's name");
}

TEST_CASE("dynamic: field with single meta", "[type_def][dynamic][field_meta]") {
    auto t = type_def("CLI")
        .field<bool>("verbose", false,
            with<cli_meta>({.cli = {.short_flag = 'v'}}));

    REQUIRE(t.field("verbose").has_meta<cli_meta>());
    REQUIRE(t.field("verbose").meta<cli_meta>().cli.short_flag == 'v');
}

// ═════════════════════════════════════════════════════════════════════════
// Field with multiple metas
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("hybrid: field with multiple metas", "[type_def][hybrid][field_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::age, "age",
            with<help_info>({.summary = "Age in years"}),
            with<render_meta>({.render = {.style = "bold", .width = 5}}));

    REQUIRE(t.field("age").has_meta<help_info>());
    REQUIRE(t.field("age").has_meta<render_meta>());
    REQUIRE(std::string_view{t.field("age").meta<help_info>().summary} == "Age in years");
    REQUIRE(std::string_view{t.field("age").meta<render_meta>().render.style} == "bold");
    REQUIRE(t.field("age").meta<render_meta>().render.width == 5);
}

TEST_CASE("dynamic: field with multiple metas", "[type_def][dynamic][field_meta]") {
    auto t = type_def("CLI")
        .field<bool>("verbose", false,
            with<cli_meta>({.cli = {.short_flag = 'v'}}),
            with<render_meta>({.render = {.style = "dim", .width = 5}}));

    REQUIRE(t.field("verbose").has_meta<cli_meta>());
    REQUIRE(t.field("verbose").has_meta<render_meta>());
    REQUIRE(t.field("verbose").meta<cli_meta>().cli.short_flag == 'v');
    REQUIRE(std::string_view{t.field("verbose").meta<render_meta>().render.style} == "dim");
    REQUIRE(t.field("verbose").meta<render_meta>().render.width == 5);
}

// ═════════════════════════════════════════════════════════════════════════
// Field without meta
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: for_each_field — query has no meta", "[type_def][typed][field_meta]") {
    bool query_has_cli = true;

    type_def<CliArgs>{}.for_each_field([&](auto descriptor) {
        if (descriptor.name() == "query") {
            query_has_cli = descriptor.template has_meta<cli_meta>();
        }
    });

    REQUIRE(!query_has_cli);
}

TEST_CASE("hybrid: field without meta", "[type_def][hybrid][field_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name");

    REQUIRE(!t.field("name").has_meta<help_info>());
}

TEST_CASE("dynamic: field without meta returns false", "[type_def][dynamic][field_meta]") {
    auto t = type_def("Event")
        .field<std::string>("title");

    REQUIRE(!t.field("title").has_meta<cli_meta>());
}

// ═════════════════════════════════════════════════════════════════════════
// meta_count and metas on fields
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("dynamic: field meta_count and metas", "[type_def][dynamic][field_meta]") {
    auto t = type_def("Event")
        .field<std::string>("title", std::string(""),
            with<tag_info>({.value = "a"}),
            with<tag_info>({.value = "b"}));

    REQUIRE(t.field("title").meta_count<tag_info>() == 2);
    auto tags = t.field("title").metas<tag_info>();
    REQUIRE(tags.size() == 2);
    REQUIRE(std::string_view{tags[0].value} == "a");
    REQUIRE(std::string_view{tags[1].value} == "b");
}

// ═════════════════════════════════════════════════════════════════════════
// for_each_field reads meta values
// ═════════════════════════════════════════════════════════════════════════

TEST_CASE("typed: for_each_field reads meta values via meta<M>()", "[type_def][typed][field_meta]") {
    char verbose_flag = '\0';

    type_def<CliArgs>{}.for_each_field([&](auto descriptor) {
        if constexpr (requires { descriptor.index(); }) {
            if (descriptor.name() == "verbose") {
                if constexpr (descriptor.template has_meta<cli_meta>()) {
                    verbose_flag = descriptor.template meta<cli_meta>().cli.short_flag;
                }
            }
        }
    });

    REQUIRE(verbose_flag == 'v');
}

TEST_CASE("hybrid: for_each_field reads meta values", "[type_def][hybrid][field_meta]") {
    auto t = type_def<PlainDog>()
        .field(&PlainDog::name, "name",
            with<help_info>({.summary = "Dog's name"}));

    std::string summary;
    t.for_each_field([&](auto descriptor) {
        if (descriptor.name() == "name" && descriptor.template has_meta<help_info>())
            summary = descriptor.template meta<help_info>().summary;
    });

    REQUIRE(summary == "Dog's name");
}
