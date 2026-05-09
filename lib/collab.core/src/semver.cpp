module;

#include <algorithm>
#include <charconv>
#include <compare>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

module collab.core;

import :semver;

namespace collab::core {

namespace {

    std::vector<std::string_view> split_identifiers(std::string_view s) {
        std::vector<std::string_view> out;
        std::size_t                   start = 0;
        for (std::size_t i = 0; i <= s.size(); ++i) {
            if (i == s.size() || s[i] == '.') {
                out.emplace_back(s.substr(start, i - start));
                start = i + 1;
            }
        }
        return out;
    }

    bool is_numeric_identifier(std::string_view id) {
        if (id.empty()) return false;
        for (char c : id) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }

    unsigned long long parse_numeric(std::string_view id) {
        unsigned long long result = 0;
        std::from_chars(id.data(), id.data() + id.size(), result);
        return result;
    }

    // Pre-release precedence per SemVer §11. Caller guarantees both are non-empty.
    std::strong_ordering compare_pre_release(std::string_view a, std::string_view b) {
        auto ids_a = split_identifiers(a);
        auto ids_b = split_identifiers(b);

        const std::size_t n = std::min(ids_a.size(), ids_b.size());
        for (std::size_t i = 0; i < n; ++i) {
            const auto xa = ids_a[i];
            const auto xb = ids_b[i];
            const bool na = is_numeric_identifier(xa);
            const bool nb = is_numeric_identifier(xb);

            if (na && nb) {
                const auto va = parse_numeric(xa);
                const auto vb = parse_numeric(xb);
                if (auto cmp = va <=> vb; cmp != 0) return cmp;
            } else if (na && !nb) {
                // §11: numeric identifiers rank below alphanumeric ones.
                return std::strong_ordering::less;
            } else if (!na && nb) {
                return std::strong_ordering::greater;
            } else {
                if (const auto cmp = xa.compare(xb); cmp != 0) {
                    return cmp < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
                }
            }
        }

        // §11: when all common identifiers are equal, the shorter list is less.
        if (ids_a.size() < ids_b.size()) return std::strong_ordering::less;
        if (ids_a.size() > ids_b.size()) return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }

}  // namespace

std::string semver::to_string() const {
    std::string out;
    out.reserve(16);
    out += std::to_string(major);
    out += '.';
    out += std::to_string(minor);
    out += '.';
    out += std::to_string(patch);
    if (!pre_release.empty()) {
        out += '-';
        out += pre_release;
    }
    if (!build.empty()) {
        out += '+';
        out += build;
    }
    return out;
}

std::strong_ordering semver::operator<=>(const semver& other) const {
    if (const auto cmp = major <=> other.major; cmp != 0) return cmp;
    if (const auto cmp = minor <=> other.minor; cmp != 0) return cmp;
    if (const auto cmp = patch <=> other.patch; cmp != 0) return cmp;

    // §11: a version without a pre-release outranks one with a pre-release.
    const bool a_has = !pre_release.empty();
    const bool b_has = !other.pre_release.empty();
    if (!a_has && !b_has) return std::strong_ordering::equal;
    if (!a_has && b_has) return std::strong_ordering::greater;
    if (a_has && !b_has) return std::strong_ordering::less;
    return compare_pre_release(pre_release, other.pre_release);
}

bool semver::operator==(const semver& other) const {
    // §10: `build` is excluded from version precedence.
    return major == other.major
        && minor == other.minor
        && patch == other.patch
        && pre_release == other.pre_release;
}

}  // namespace collab::core
