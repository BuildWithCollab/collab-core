module;

#include <compare>
#include <string>

export module collab.core:semver;

export namespace collab::core {

// Semantic version per SemVer 2.0 (https://semver.org).
struct semver {
    int         major = 0;
    int         minor = 0;
    int         patch = 0;
    std::string pre_release;
    std::string build;

    std::string to_string() const;

    // Both hand-written: defaulting either would include `build`, which
    // SemVer §10 requires to be ignored when determining precedence.
    std::strong_ordering operator<=>(const semver&) const;
    bool                 operator==(const semver&) const;
};

}  // namespace collab::core
