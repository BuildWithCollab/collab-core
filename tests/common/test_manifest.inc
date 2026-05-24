// Manifest tests — header-only-compatible.

TEST_CASE("manifest: composes identifier + semver + optional metadata", "[manifest]") {
    collab::manifest m{
        .identifier = {
            .app_id   = "collab-net",
            .app_name = "Collab Net",
            .org_id   = "mrowrpurr",
            .org_name = "Mrowr Purr",
            .tld      = "com",
        },
        .version     = {0, 2, 0},
        .description = "Networking layer.",
        .authors     = {"Mrowr Purr"},
        .license     = "0BSD",
    };
    REQUIRE(m.identifier.bundle_id() == "com.mrowrpurr.collab-net");
    REQUIRE(m.version.to_string() == "0.2.0");
    REQUIRE(m.description.has_value());
    REQUIRE(*m.description == "Networking layer.");
    REQUIRE(m.authors.size() == 1);
    REQUIRE(m.authors[0] == "Mrowr Purr");
    REQUIRE(m.license.has_value());
    REQUIRE(*m.license == "0BSD");
}

TEST_CASE("manifest: absent optionals are distinct from empty values", "[manifest]") {
    collab::manifest m{
        .identifier = {},
        .version    = {},
    };
    REQUIRE_FALSE(m.description.has_value());
    REQUIRE_FALSE(m.license.has_value());
    REQUIRE(m.authors.empty());
}
