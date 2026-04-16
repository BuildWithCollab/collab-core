module;

#include <array>
#include <cstddef>
#include <concepts>
#include <string_view>
#include <type_traits>
#include <utility>

export module collab.core:field;

export namespace collab::model {

// ── with<Exts...> ───────────────────────────────────────────────────────

template <typename... Exts>
struct with : Exts... {};

// ── field<T, WithPack> ─────────────────────────────────────────────────

template <typename T, typename WithPack = with<>>
struct field {
    using value_type = T;

    WithPack    with{};
    T           value{};

    constexpr operator const T&() const& noexcept { return value; }
    constexpr operator       T&()       & noexcept { return value; }
    constexpr operator       T&&()      && noexcept { return std::move(value); }

    constexpr field& operator=(const T& v) {
        value = v;
        return *this;
    }
    constexpr field& operator=(T&& v) noexcept(std::is_nothrow_move_assignable_v<T>) {
        value = std::move(v);
        return *this;
    }

    constexpr const T* operator->() const noexcept { return &value; }
    constexpr       T* operator->()       noexcept { return &value; }
};

// ── is_field concept ─────────────────────────────────────────────────────

namespace detail {

    template <typename U>
    inline constexpr bool is_field_v = false;

    template <typename T, typename W>
    inline constexpr bool is_field_v<field<T, W>> = true;

}  // namespace detail

template <typename T>
concept is_field = detail::is_field_v<std::remove_cvref_t<T>>;

// ── reflect_on<T>() — user registration point ───────────────────────────
//
// Specialize this for your type to register it for reflection.
// The primary template is deleted — unspecialized types fall through
// to the PFR backend (if enabled) or hit a static_assert.
//
// Example:
//   template <>
//   constexpr auto collab::model::reflect_on<MyArgs>() {
//       return collab::model::field_info<MyArgs>("name", "age", "active");
//   }

template <typename T>
constexpr auto reflect_on() = delete;

// ── field_info — registration helper ────────────────────────────────────

template <std::size_t N>
struct field_names_t {
    std::array<std::string_view, N> names{};
};

template <typename T, typename... Strs>
consteval auto field_info(Strs... names) -> field_names_t<sizeof...(Strs)> {
    return {.names = {std::string_view(names)...}};
}

// ── has_reflect_on detection ──────────────────────────────────────────────

namespace detail {

    template <typename T>
    concept has_reflect_on = requires { reflect_on<T>(); };

}  // namespace detail

}  // namespace collab::model
