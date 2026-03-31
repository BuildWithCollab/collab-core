#pragma once

#include <compare>
#include <string>

namespace collab {

struct Version {
    int major = 0;
    int minor = 0;
    int patch = 0;

    constexpr auto operator<=>(const Version&) const = default;

    auto to_string() const -> std::string;
};

}  // namespace collab
