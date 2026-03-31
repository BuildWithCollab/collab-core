#include "collab/version.hpp"

#include <string>

namespace collab {

auto Version::to_string() const -> std::string {
    return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
}

}  // namespace collab
