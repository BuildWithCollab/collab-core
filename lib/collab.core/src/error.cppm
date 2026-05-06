module;

#include <string>
#include <string_view>

export module collab.core:error;

export namespace collab::core {

enum class Category {
    other,
    usage,
    not_found,
    permission,
    unauthenticated,
    network,
    timeout,
    cancelled,
    conflict,
    unavailable,
};

struct Error {
    Category    category;
    std::string message;

    Error(Category c, std::string_view m) : category{c}, message{m} {}
};

}  // namespace collab::core
