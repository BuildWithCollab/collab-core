#include <catch2/catch_test_macros.hpp>

#include <compare>

import collab.core;

using collab::core::semver;

TEST_CASE("collab::core::version is defined", "[version]") {
    REQUIRE(collab::core::version.major == 1);
    REQUIRE(collab::core::version.minor == 0);
    REQUIRE(collab::core::version.patch == 0);
}

TEST_CASE("semver construction", "[semver]") {
    const semver v1{1, 2, 3};
    REQUIRE(v1.major == 1);
    REQUIRE(v1.minor == 2);
    REQUIRE(v1.patch == 3);
    REQUIRE(v1.pre_release.empty());
    REQUIRE(v1.build.empty());

    const semver v2{1, 0, 0, "alpha.1", "build.42"};
    REQUIRE(v2.pre_release == "alpha.1");
    REQUIRE(v2.build == "build.42");
}

TEST_CASE("semver to_string produces canonical form", "[semver][to_string]") {
    REQUIRE(semver{1, 2, 3}.to_string() == "1.2.3");
    REQUIRE(semver{0, 0, 0}.to_string() == "0.0.0");
    REQUIRE(semver{1, 0, 0, "alpha.1"}.to_string() == "1.0.0-alpha.1");
    REQUIRE(semver{1, 0, 0, "", "build.42"}.to_string() == "1.0.0+build.42");
    REQUIRE(semver{1, 0, 0, "alpha.1", "build.42"}.to_string() == "1.0.0-alpha.1+build.42");
}

TEST_CASE("semver ordering on major/minor/patch", "[semver][ordering]") {
    REQUIRE(semver{1, 2, 3} < semver{1, 2, 4});
    REQUIRE(semver{1, 2, 3} < semver{1, 3, 0});
    REQUIRE(semver{1, 2, 3} < semver{2, 0, 0});
    REQUIRE(semver{2, 0, 0} > semver{1, 99, 99});
    REQUIRE(semver{1, 2, 3} == semver{1, 2, 3});
}

TEST_CASE("semver pre-release precedence per SemVer 2.0 §11", "[semver][ordering][prerelease]") {
    REQUIRE(semver{1, 0, 0, "alpha"} < semver{1, 0, 0});
    REQUIRE(semver{1, 0, 0} > semver{1, 0, 0, "alpha"});

    REQUIRE(semver{1, 0, 0, "alpha"} < semver{1, 0, 0, "beta"});
    REQUIRE(semver{1, 0, 0, "alpha"} < semver{1, 0, 0, "alpha.1"});
    REQUIRE(semver{1, 0, 0, "alpha.1"} < semver{1, 0, 0, "alpha.beta"});

    REQUIRE(semver{1, 0, 0, "alpha.1"} < semver{1, 0, 0, "alpha.2"});
    REQUIRE(semver{1, 0, 0, "alpha.2"} < semver{1, 0, 0, "alpha.10"});  // fails under lex comparison

    // Canonical §11 ladder.
    REQUIRE(semver{1, 0, 0, "alpha"} < semver{1, 0, 0, "alpha.1"});
    REQUIRE(semver{1, 0, 0, "alpha.1"} < semver{1, 0, 0, "alpha.beta"});
    REQUIRE(semver{1, 0, 0, "alpha.beta"} < semver{1, 0, 0, "beta"});
    REQUIRE(semver{1, 0, 0, "beta"} < semver{1, 0, 0, "beta.2"});
    REQUIRE(semver{1, 0, 0, "beta.2"} < semver{1, 0, 0, "beta.11"});
    REQUIRE(semver{1, 0, 0, "beta.11"} < semver{1, 0, 0, "rc.1"});
    REQUIRE(semver{1, 0, 0, "rc.1"} < semver{1, 0, 0});
}

TEST_CASE("semver ignores build metadata in both == and <=> per SemVer §10", "[semver][build]") {
    REQUIRE(semver{1, 0, 0, "", "build.1"} == semver{1, 0, 0, "", "build.2"});
    REQUIRE((semver{1, 0, 0, "", "build.1"} <=> semver{1, 0, 0, "", "build.2"}) == std::strong_ordering::equal);

    REQUIRE(semver{1, 0, 0, "alpha.1", "build.1"} == semver{1, 0, 0, "alpha.1", "build.2"});
    REQUIRE(semver{1, 0, 0, "alpha.1", "build.1"} < semver{1, 0, 0, "alpha.2", "build.0"});

    REQUIRE(semver{1, 0, 0, "", "a"} < semver{1, 0, 1, "", "z"});
}
