module;

#include <string>
#include <string_view>

export module collab.core:error;

export namespace collab::core {

struct error {
    std::string message;
    std::string category = "general";
};

namespace error_category {
    inline constexpr std::string_view general     = "general";
    inline constexpr std::string_view usage       = "usage";
    inline constexpr std::string_view not_found   = "not_found";
    inline constexpr std::string_view permission  = "permission";
    inline constexpr std::string_view network     = "network";
    inline constexpr std::string_view timeout     = "timeout";
    inline constexpr std::string_view conflict    = "conflict";
    inline constexpr std::string_view unavailable = "unavailable";
    inline constexpr std::string_view internal    = "internal";
}  // namespace error_category

}  // namespace collab::core
