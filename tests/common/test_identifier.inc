// Identifier tests — header-only-compatible.

TEST_CASE("identifier: bundle_id is tld.org_id.app_id", "[identifier]") {
    collab::identifier id{
        .app_id   = "collab-core",
        .app_name = "Collab Core",
        .org_id   = "mrowrpurr",
        .org_name = "Mrowr Purr",
        .tld      = "com",
    };
    REQUIRE(id.bundle_id() == "com.mrowrpurr.collab-core");
}

TEST_CASE("identifier: empty fields produce a still-well-formed bundle_id", "[identifier]") {
    collab::identifier id{};
    REQUIRE(id.bundle_id() == "..");
}
