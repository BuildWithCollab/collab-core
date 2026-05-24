// Semver tests — header-only-compatible.

TEST_CASE("semver: to_string canonical forms", "[semver]") {
    REQUIRE(collab::semver{1, 2, 3}.to_string() == "1.2.3");
    REQUIRE(collab::semver{1, 0, 0, "rc.1"}.to_string() == "1.0.0-rc.1");
    REQUIRE(collab::semver{1, 0, 0, "", "20240101"}.to_string() == "1.0.0+20240101");
    REQUIRE(collab::semver{1, 0, 0, "beta.2", "sha.abc"}.to_string() == "1.0.0-beta.2+sha.abc");
}

TEST_CASE("semver: precedence by major/minor/patch", "[semver]") {
    REQUIRE(collab::semver{1, 0, 0} < collab::semver{2, 0, 0});
    REQUIRE(collab::semver{1, 0, 0} < collab::semver{1, 1, 0});
    REQUIRE(collab::semver{1, 1, 0} < collab::semver{1, 1, 1});
}

TEST_CASE("semver: pre-release precedes the release", "[semver]") {
    REQUIRE(collab::semver{1, 0, 0, "alpha"} < collab::semver{1, 0, 0});
    REQUIRE(collab::semver{1, 0, 0, "rc.1"} < collab::semver{1, 0, 0});
}

TEST_CASE("semver: pre-release precedence per SemVer §11", "[semver]") {
    using V = collab::semver;
    REQUIRE(V{1, 0, 0, "alpha"}      < V{1, 0, 0, "alpha.1"});
    REQUIRE(V{1, 0, 0, "alpha.1"}    < V{1, 0, 0, "alpha.beta"});
    REQUIRE(V{1, 0, 0, "alpha.beta"} < V{1, 0, 0, "beta"});
    REQUIRE(V{1, 0, 0, "beta"}       < V{1, 0, 0, "beta.2"});
    REQUIRE(V{1, 0, 0, "beta.2"}     < V{1, 0, 0, "beta.11"});  // numeric, not lexical
    REQUIRE(V{1, 0, 0, "beta.11"}    < V{1, 0, 0, "rc.1"});
}

TEST_CASE("semver: build metadata is ignored for precedence (§10)", "[semver]") {
    REQUIRE(collab::semver{1, 0, 0, "", "build.1"} == collab::semver{1, 0, 0, "", "build.999"});
    REQUIRE_FALSE(collab::semver{1, 0, 0, "", "build.1"} <  collab::semver{1, 0, 0, "", "build.999"});
    REQUIRE_FALSE(collab::semver{1, 0, 0, "", "build.1"} >  collab::semver{1, 0, 0, "", "build.999"});
}

TEST_CASE("semver: equality compares fields without build", "[semver]") {
    REQUIRE(collab::semver{1, 2, 3, "rc.1"} == collab::semver{1, 2, 3, "rc.1"});
    REQUIRE_FALSE(collab::semver{1, 2, 3} == collab::semver{1, 2, 4});
    REQUIRE_FALSE(collab::semver{1, 2, 3, "rc.1"} == collab::semver{1, 2, 3, "rc.2"});
}
