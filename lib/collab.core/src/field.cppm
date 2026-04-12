module;

#include <boost/pfr.hpp>

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

export module collab.core:field;

export namespace collab::core::fields {

// ── with<Exts...> ───────────────────────────────────────────────────────
//
// Bundles extension structs into a single aggregate that Field holds as a
// direct member. Each Ext should be a small struct wrapping one subsystem's
// metadata — e.g. `struct posix_meta { posix::options posix{}; };`.
//
// Inheriting from Exts... means:
//   • Duplicate extension types are a compile error (can't inherit the
//     same base twice) — no static_assert needed.
//   • Extension members are accessible via the subsystem's member name
//     (`.posix`, `.render`, etc.) after drilling through `.with`.
//
// Why the wrapper exists: MSVC rejects mixing designated and positional
// initializers in the same brace-enclosed list. Without `with`, Field
// would inherit from Exts... directly, forcing positional base init that
// can't coexist with designated member init. Wrapping all bases into one
// `.with` member keeps every Field-level init designated.
template <typename... Exts>
struct with : Exts... {};

// ── Field<T, WithPack> ─────────────────────────────────────────────────
//
// The reflection primitive. Wraps a value of type T with optional inline
// metadata (desc, required, display_name, hidden) and an extensible
// `with<Exts...>` pack for domain-specific data (CLI, render, DB, etc.).
//
// Field is parameterized only by T and WithPack (both plain types). No
// class-type NTTPs touch its specialization, so MSVC 14.50's duplicate-
// COMDAT bug (LNK1179) cannot fire.
//
// The field's compile-time name comes from boost::pfr::get_name on the
// outer struct, not from a template parameter. Users write the member
// name once as the C++ identifier and never duplicate it as a string.
template <typename T, typename WithPack = with<>>
struct Field {
    WithPack    with{};
    T           value{};

    // ── Transparent conversions ─────────────────────────────────────
    constexpr operator const T&() const& noexcept { return value; }
    constexpr operator       T&()       & noexcept { return value; }
    constexpr operator       T&&()      && noexcept { return std::move(value); }

    // ── Transparent assignment ──────────────────────────────────────
    constexpr Field& operator=(const T& v) {
        value = v;
        return *this;
    }
    constexpr Field& operator=(T&& v) noexcept(std::is_nothrow_move_assignable_v<T>) {
        value = std::move(v);
        return *this;
    }

    // ── Method-call passthrough ─────────────────────────────────────
    constexpr const T* operator->() const noexcept { return &value; }
    constexpr       T* operator->()       noexcept { return &value; }
};

// ── IsField concept ─────────────────────────────────────────────────────

namespace detail {

    template <typename U>
    inline constexpr bool is_field_v = false;

    template <typename T, typename W>
    inline constexpr bool is_field_v<Field<T, W>> = true;

}  // namespace detail

template <typename T>
concept IsField = detail::is_field_v<std::remove_cvref_t<T>>;

// ── ReflectedStruct concept ─────────────────────────────────────────────

namespace detail {

    template <typename T, std::size_t... Is>
    consteval bool any_member_is_field(std::index_sequence<Is...>) {
        return (IsField<boost::pfr::tuple_element_t<Is, T>> || ...);
    }

    template <typename T>
    consteval bool has_any_field_member() {
        if constexpr (!std::is_aggregate_v<T>) {
            return false;
        } else if constexpr (boost::pfr::tuple_size_v<T> == std::size_t{0}) {
            return false;
        } else {
            return any_member_is_field<T>(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});
        }
    }

}  // namespace detail

template <typename T>
concept ReflectedStruct =
    std::is_aggregate_v<T>
    && detail::has_any_field_member<T>();

}  // namespace collab::core::fields
