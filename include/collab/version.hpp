#pragma once

#include <compare>
#include <string_view>

namespace collab {

struct Version {
    int major = 0;
    int minor = 0;
    int patch = 0;

    constexpr auto operator<=>(const Version&) const = default;
};

// collab-core version
inline constexpr Version core_version{0, 4, 0};
inline constexpr std::string_view core_version_string = "0.4.0";

}  // namespace collab
