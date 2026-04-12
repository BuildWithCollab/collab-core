// [EXPERIMENT O] Combine: alias-template `name<S>` + type-param Core tag
// (user-declared once). Real-shape DSL.
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

import collab.core;

using collab::core::fixed_string;
using collab::core::options;

template <char... Chars>
struct name_pack {
    static constexpr char value[sizeof...(Chars) + 1] = {Chars..., '\0'};
};

template <fixed_string S, std::size_t... Is>
consteval auto to_name_pack_impl(std::index_sequence<Is...>) -> name_pack<S.value[Is]...> {
    return {};
}

template <fixed_string S>
using name = decltype(to_name_pack_impl<S>(std::make_index_sequence<S.size()>{}));

struct default_core_tag {
    static constexpr options value{};
};

template <typename NamePack, typename T, typename CoreTag = default_core_tag>
struct MiniField {
    T value{};
    static consteval const char*  field_name() { return NamePack::value; }
    static consteval const auto&  field_core() { return CoreTag::value; }
};

// User declares tag structs once for each distinct options value.
struct session_id_meta {
    static constexpr options value = {.desc = "Session ID", .required = true};
};

struct LoginResponseO  { MiniField<name<"session_id">, std::string, session_id_meta> session_id; };
struct LogoutResponseO { MiniField<name<"session_id">, std::string, session_id_meta> session_id; };
struct WhoAmIO         { MiniField<name<"session_id">, std::string, session_id_meta> session_id; };

// Also test plain default-options inline — no tag needed:
struct UserProfileO {
    MiniField<name<"first">, std::string> first;
    MiniField<name<"last">,  std::string> last;
    MiniField<name<"age">,   int>         age;
};

struct EmployeeO {
    MiniField<name<"first">, std::string> first;   // same spec as UserProfile.first
    MiniField<name<"last">,  std::string> last;    // same spec as UserProfile.last
};

TEST_CASE("experiment O: full DSL shape", "[exp-o]") {
    LoginResponseO  login{};
    LogoutResponseO logout{};
    WhoAmIO         whoami{};
    UserProfileO    user{};
    EmployeeO       emp{};

    REQUIRE(login.session_id.value.empty());
    REQUIRE(logout.session_id.value.empty());
    REQUIRE(whoami.session_id.value.empty());

    using F = decltype(login.session_id);
    const char* n = F::field_name();
    REQUIRE(std::string_view{n} == "session_id");
    REQUIRE(std::string_view{F::field_core().desc} == "Session ID");
    REQUIRE(F::field_core().required == true);

    // Default-core case, same spec used twice across structs:
    REQUIRE(user.first.value.empty());
    REQUIRE(emp.first.value.empty());
    REQUIRE(user.age.value == 0);
}
