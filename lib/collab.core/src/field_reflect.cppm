module;

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#ifdef COLLAB_FIELD_HAS_PFR
#include <pfr.hpp>
#endif

export module collab.core:field_reflect;

import :field;

// ════════════════════════════════════════════════════════════════════════════
// Section 1 — PFR backend (opt-in via COLLAB_FIELD_HAS_PFR)
// ════════════════════════════════════════════════════════════════════════════

#ifdef COLLAB_FIELD_HAS_PFR

namespace collab::field::pfr_impl {

template <typename T>
consteval std::size_t field_count() {
    return pfr::tuple_size_v<T>;
}

template <std::size_t I, typename T>
consteval std::string_view field_name() {
    return pfr::get_name<I, T>();
}

// Runtime field name extraction — completely avoids consteval to work around
// a clang bug where consteval results get assigned to the wrong template
// instantiation across C++20 module boundaries.
//
// Uses __PRETTY_FUNCTION__ parsing at runtime (same technique as PFR, but
// evaluated at runtime instead of compile time).
template <auto ptr>
const char* name_of_field_pretty_fn() {
    return __PRETTY_FUNCTION__;
}

template <auto ptr>
std::string_view name_of_field_rt() {
    static const std::string_view result = [] {
        std::string_view sv = name_of_field_pretty_fn<ptr>();
        // Find last "." before the closing "}]" and take everything after it
        auto end = sv.rfind("}]");
        if (end == std::string_view::npos) end = sv.size();
        auto sub = sv.substr(0, end);
        auto dot = sub.rfind('.');
        if (dot == std::string_view::npos) return sv; // fallback
        return sub.substr(dot + 1);
    }();
    return result;
}

template <std::size_t I, typename T>
std::string_view field_name_rt() {
    return name_of_field_rt<
        pfr::detail::make_clang_wrapper(std::addressof(pfr::detail::sequence_tuple::get<I>(
            pfr::detail::tie_as_tuple(pfr::detail::fake_object<T>())
        )))
    >();
}

template <std::size_t I, typename T>
using member_type = pfr::tuple_element_t<I, T>;

template <std::size_t I, typename T>
constexpr decltype(auto) get_member(T& obj) {
    return pfr::get<I>(obj);
}

template <typename T, typename F>
constexpr void for_each_member(T& obj, F&& fn) {
    pfr::for_each_field(obj, std::forward<F>(fn));
}

}  // namespace collab::field::pfr_impl

#endif

// ════════════════════════════════════════════════════════════════════════════
// Section 2 — Registry backend (structured-binding decomposition)
// ════════════════════════════════════════════════════════════════════════════

namespace collab::field::registry {

// Decomposes aggregates via structured bindings up to 16 members.
// This is the non-PFR backend — works on any C++23 compiler.

template <std::size_t I, typename T>
constexpr decltype(auto) get_member(T& obj) {
    constexpr auto N = reflect_on<std::remove_cvref_t<T>>().names.size();
    if constexpr (N == 1) {
        auto& [m0] = obj;
        if constexpr (I == 0) return (m0);
    } else if constexpr (N == 2) {
        auto& [m0, m1] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
    } else if constexpr (N == 3) {
        auto& [m0, m1, m2] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
    } else if constexpr (N == 4) {
        auto& [m0, m1, m2, m3] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
    } else if constexpr (N == 5) {
        auto& [m0, m1, m2, m3, m4] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
    } else if constexpr (N == 6) {
        auto& [m0, m1, m2, m3, m4, m5] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
    } else if constexpr (N == 7) {
        auto& [m0, m1, m2, m3, m4, m5, m6] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
    } else if constexpr (N == 8) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
    } else if constexpr (N == 9) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
    } else if constexpr (N == 10) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
        else if constexpr (I == 9) return (m9);
    } else if constexpr (N == 11) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
        else if constexpr (I == 9) return (m9);
        else if constexpr (I == 10) return (m10);
    } else if constexpr (N == 12) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
        else if constexpr (I == 9) return (m9);
        else if constexpr (I == 10) return (m10);
        else if constexpr (I == 11) return (m11);
    } else if constexpr (N == 13) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
        else if constexpr (I == 9) return (m9);
        else if constexpr (I == 10) return (m10);
        else if constexpr (I == 11) return (m11);
        else if constexpr (I == 12) return (m12);
    } else if constexpr (N == 14) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
        else if constexpr (I == 9) return (m9);
        else if constexpr (I == 10) return (m10);
        else if constexpr (I == 11) return (m11);
        else if constexpr (I == 12) return (m12);
        else if constexpr (I == 13) return (m13);
    } else if constexpr (N == 15) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
        else if constexpr (I == 9) return (m9);
        else if constexpr (I == 10) return (m10);
        else if constexpr (I == 11) return (m11);
        else if constexpr (I == 12) return (m12);
        else if constexpr (I == 13) return (m13);
        else if constexpr (I == 14) return (m14);
    } else if constexpr (N == 16) {
        auto& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15] = obj;
        if constexpr (I == 0) return (m0);
        else if constexpr (I == 1) return (m1);
        else if constexpr (I == 2) return (m2);
        else if constexpr (I == 3) return (m3);
        else if constexpr (I == 4) return (m4);
        else if constexpr (I == 5) return (m5);
        else if constexpr (I == 6) return (m6);
        else if constexpr (I == 7) return (m7);
        else if constexpr (I == 8) return (m8);
        else if constexpr (I == 9) return (m9);
        else if constexpr (I == 10) return (m10);
        else if constexpr (I == 11) return (m11);
        else if constexpr (I == 12) return (m12);
        else if constexpr (I == 13) return (m13);
        else if constexpr (I == 14) return (m14);
        else if constexpr (I == 15) return (m15);
    } else {
        static_assert(N <= 16, "Registry backend supports up to 16 fields. Use PFR for more.");
    }
}

template <std::size_t I, typename T>
using member_type = std::remove_cvref_t<decltype(get_member<I>(std::declval<T&>()))>;

template <typename T, typename F, std::size_t... Is>
constexpr void for_each_member_impl(T& obj, F&& fn, std::index_sequence<Is...>) {
    (fn(get_member<Is>(obj), std::integral_constant<std::size_t, Is>{}), ...);
}

template <typename T, typename F>
constexpr void for_each_member(T& obj, F&& fn) {
    constexpr auto N = reflect_on<std::remove_cvref_t<T>>().names.size();
    for_each_member_impl(obj, std::forward<F>(fn), std::make_index_sequence<N>{});
}

}  // namespace collab::field::registry

// ════════════════════════════════════════════════════════════════════════════
// Section 3 — Exported API (dispatch logic, reflection, concepts, helpers)
// ════════════════════════════════════════════════════════════════════════════

export namespace collab::field {

// ── type_name<T>() ──────────────────────────────────────────────────────

namespace detail {

    template <typename T>
    consteval std::string_view extract_type_name() {
#if defined(_MSC_VER)
        std::string_view sv = __FUNCSIG__;
        auto start = sv.find("extract_type_name<") + 18;
        if (sv.substr(start, 7) == "struct ") start += 7;
        else if (sv.substr(start, 6) == "class ")  start += 6;
        else if (sv.substr(start, 5) == "enum ")   start += 5;
        auto end = sv.rfind(">(void)");
        return sv.substr(start, end - start);
#elif defined(__clang__)
        std::string_view sv = __PRETTY_FUNCTION__;
        auto start = sv.find("T = ") + 4;
        auto end = sv.rfind("]");
        return sv.substr(start, end - start);
#elif defined(__GNUC__)
        std::string_view sv = __PRETTY_FUNCTION__;
        auto start = sv.find("T = ") + 4;
        auto end = sv.find(";", start);
        if (end == std::string_view::npos) end = sv.rfind("]");
        return sv.substr(start, end - start);
#else
        return "unknown";
#endif
    }

}  // namespace detail

template <typename T>
consteval std::string_view type_name() {
    return detail::extract_type_name<T>();
}

// ── Backend dispatch ────────────────────────────────────────────────────
//
// One ifdef picks the fallback. Everything else is clean C++.

#ifdef COLLAB_FIELD_HAS_PFR
inline constexpr bool has_pfr_backend = true;
#else
inline constexpr bool has_pfr_backend = false;
#endif

namespace detail {

    // ── Fallback backend ─────────────────────────────────────────────
    //
    // When PFR is enabled, the fallback delegates to pfr_impl.
    // When PFR is disabled, every method static_asserts.

    template <bool PfrEnabled>
    struct fallback;

    template <>
    struct fallback<false> {
        template <typename T>
        static consteval std::size_t field_count() {
            static_assert(has_reflect_on<T>,
                "Type has no reflect_on<T>() specialization and PFR is not enabled.");
            return 0;
        }

        template <std::size_t I, typename T>
        static consteval std::string_view field_name() {
            static_assert(has_reflect_on<T>,
                "Type has no reflect_on<T>() specialization and PFR is not enabled.");
            return "";
        }

        template <std::size_t I, typename T>
        static std::string_view field_name_rt() {
            static_assert(has_reflect_on<T>,
                "Type has no reflect_on<T>() specialization and PFR is not enabled.");
            return "";
        }

        template <std::size_t I, typename T>
        using member_type = void;

        template <std::size_t I, typename T>
        static constexpr decltype(auto) get_member(T&) {
            static_assert(has_reflect_on<std::remove_cvref_t<T>>,
                "Type has no reflect_on<T>() specialization and PFR is not enabled.");
        }

        template <typename T, typename F>
        static constexpr void for_each_member(T&, F&&) {
            static_assert(has_reflect_on<std::remove_cvref_t<T>>,
                "Type has no reflect_on<T>() specialization and PFR is not enabled.");
        }
    };

#ifdef COLLAB_FIELD_HAS_PFR
    template <>
    struct fallback<true> {
        template <typename T>
        static consteval std::size_t field_count() {
            return pfr_impl::field_count<T>();
        }

        template <std::size_t I, typename T>
        static consteval std::string_view field_name() {
            return pfr_impl::field_name<I, T>();
        }

        template <std::size_t I, typename T>
        static std::string_view field_name_rt() {
            return pfr_impl::field_name_rt<I, T>();
        }

        template <std::size_t I, typename T>
        using member_type = pfr_impl::member_type<I, T>;

        template <std::size_t I, typename T>
        static constexpr decltype(auto) get_member(T& obj) {
            return pfr_impl::get_member<I>(obj);
        }

        template <typename T, typename F>
        static constexpr void for_each_member(T& obj, F&& fn) {
            pfr_impl::for_each_member(obj, std::forward<F>(fn));
        }
    };
#endif

    using pfr_fallback = fallback<has_pfr_backend>;

    // ── Dispatch: registry first, PFR fallback second ────────────────

    template <typename T>
    consteval std::size_t dispatch_field_count() {
        if constexpr (has_reflect_on<T>) return reflect_on<T>().names.size();
        else return pfr_fallback::template field_count<T>();
    }

    template <std::size_t I, typename T>
    consteval std::string_view dispatch_field_name() {
        if constexpr (has_reflect_on<T>) return reflect_on<T>().names[I];
        else return pfr_fallback::template field_name<I, T>();
    }

    // Runtime-safe variant: avoids consteval string_view materialization
    // to work around a clang bug where consteval string_views that reference
    // inline constexpr variable templates across module boundaries get their
    // pointer and size paired from different template instantiations.
    template <std::size_t I, typename T>
    std::string_view dispatch_field_name_rt() {
        if constexpr (has_reflect_on<T>) return reflect_on<T>().names[I];
        else return pfr_fallback::template field_name_rt<I, T>();
    }

    // Lazy member_type — only instantiates the chosen backend
    template <std::size_t I, typename T, bool Registered = has_reflect_on<T>>
    struct dispatch_member_type_impl;

    template <std::size_t I, typename T>
    struct dispatch_member_type_impl<I, T, true> {
        using type = registry::member_type<I, T>;
    };

    template <std::size_t I, typename T>
    struct dispatch_member_type_impl<I, T, false> {
        using type = typename pfr_fallback::template member_type<I, T>;
    };

    template <std::size_t I, typename T>
    using member_type = typename dispatch_member_type_impl<I, T>::type;

    template <std::size_t I, typename T>
    constexpr decltype(auto) dispatch_get_member(T& obj) {
        if constexpr (has_reflect_on<std::remove_cvref_t<T>>) return registry::get_member<I>(obj);
        else return pfr_fallback::template get_member<I>(obj);
    }

    template <typename T, typename F>
    constexpr void dispatch_for_each_member(T& obj, F&& fn) {
        if constexpr (has_reflect_on<std::remove_cvref_t<T>>) registry::for_each_member(obj, std::forward<F>(fn));
        else pfr_fallback::for_each_member(obj, std::forward<F>(fn));
    }

}  // namespace detail

// ── reflected_struct concept ─────────────────────────────────────────────

namespace detail {

    template <typename T, std::size_t... Is>
    consteval bool any_member_is_field(std::index_sequence<Is...>) {
        return (is_field<member_type<Is, T>> || ...);
    }

    template <typename T>
    consteval bool has_any_field_member() {
        if constexpr (!std::is_aggregate_v<T>) {
            return false;
        } else {
            constexpr auto N = dispatch_field_count<T>();
            if constexpr (N == 0) {
                return false;
            } else {
                return any_member_is_field<T>(std::make_index_sequence<N>{});
            }
        }
    }

    // Count only Field<> members (not all struct members)
    template <typename T, std::size_t... Is>
    consteval std::size_t count_field_members(std::index_sequence<Is...>) {
        return (0 + ... + (is_field<member_type<Is, T>> ? 1 : 0));
    }

}  // namespace detail

template <typename T>
concept reflected_struct =
    std::is_aggregate_v<T>
    && detail::has_any_field_member<T>();

// ── Free functions ──────────────────────────────────────────────────────

template <typename T>
consteval std::size_t field_count() {
    return detail::count_field_members<T>(
        std::make_index_sequence<detail::dispatch_field_count<T>()>{}
    );
}

template <std::size_t I, typename T>
consteval std::string_view field_name() {
    return detail::dispatch_field_name<I, T>();
}

namespace detail {

    // Collect only Field member names into an array
    template <typename T, std::size_t... Is>
    constexpr auto collect_field_names(std::index_sequence<Is...>) {
        constexpr std::size_t total = sizeof...(Is);
        // First pass: count Field members
        constexpr std::size_t field_n = (0 + ... + (is_field<member_type<Is, T>> ? 1 : 0));
        // Second pass: collect their names
        std::array<std::string_view, field_n> result{};
        std::size_t idx = 0;
        ((is_field<member_type<Is, T>>
            ? (result[idx++] = dispatch_field_name_rt<Is, T>(), 0) : 0), ...);
        return result;
    }

}  // namespace detail

template <typename T>
constexpr auto field_names() {
    constexpr auto N = detail::dispatch_field_count<T>();
    return detail::collect_field_names<T>(std::make_index_sequence<N>{});
}

// ── field_descriptor<T, I> — schema-only field info ─────────────────────

template <typename T, std::size_t I>
struct field_descriptor {
    std::string_view name() const {
        return detail::dispatch_field_name_rt<I, T>();
    }

    consteval std::size_t index() const { return I; }

    template <typename Ext>
    consteval bool has_extension() const {
        using member_t = detail::member_type<I, T>;
        if constexpr (is_field<member_t>) {
            using with_type = std::remove_cvref_t<decltype(std::declval<member_t>().with)>;
            return std::is_base_of_v<Ext, with_type>;
        } else {
            return false;
        }
    }

    constexpr auto value() const {
        T instance{};
        return detail::dispatch_get_member<I>(instance).value;
    }

    constexpr auto with() const {
        T instance{};
        return detail::dispatch_get_member<I>(instance).with;
    }
};

// ── bound_field<T, I, Ref> — field with instance reference ──────────────

template <typename T, std::size_t I, typename Ref>
class bound_field {
    Ref& ref_;

public:
    explicit constexpr bound_field(Ref& r) : ref_(r) {}

    std::string_view name() const {
        return detail::dispatch_field_name_rt<I, T>();
    }

    consteval std::size_t index() const { return I; }

    constexpr decltype(auto) value() const { return (ref_.value); }
    constexpr decltype(auto) value()       { return (ref_.value); }

    constexpr decltype(auto) with() const { return (ref_.with); }
    constexpr decltype(auto) with()       { return (ref_.with); }

    template <typename Ext>
    consteval bool has_extension() const {
        using with_type = std::remove_cvref_t<decltype(std::declval<Ref>().with)>;
        return std::is_base_of_v<Ext, with_type>;
    }
};

// ── Detail: iteration helpers ───────────────────────────────────────────

namespace detail {

    template <typename T, std::size_t I, typename F>
    constexpr void invoke_if_field(F&& fn) {
        using member_t = member_type<I, T>;
        if constexpr (is_field<member_t>) {
            fn(field_descriptor<T, I>{});
        }
    }

    template <typename T, typename F, std::size_t... Is>
    constexpr void for_each_field_schema(F&& fn, std::index_sequence<Is...>) {
        (invoke_if_field<T, Is>(fn), ...);
    }

}  // namespace detail

// ── reflection<T> — the view ────────────────────────────────────────────

template <typename T>
struct reflection {
    consteval std::string_view name() const {
        return type_name<T>();
    }

    // Returns only the count of Field<> members (not total struct members)
    consteval std::size_t count() const {
        return field_count<T>();
    }

    auto names() const {
        return field_names<T>();
    }

    template <std::size_t I>
    constexpr auto at() const {
        return field_descriptor<T, I>{};
    }

    // ── Instance iteration — only visits Field<> members ─────────────

    template <typename F>
    constexpr void for_each(T& obj, F&& fn) const {
        detail::dispatch_for_each_member(obj, [&]<typename M>(M& member, auto I) {
            if constexpr (is_field<std::remove_cvref_t<M>>) {
                fn(bound_field<T, I(), M>{member});
            }
        });
    }

    template <typename F>
    constexpr void for_each(const T& obj, F&& fn) const {
        detail::dispatch_for_each_member(obj, [&]<typename M>(const M& member, auto I) {
            if constexpr (is_field<std::remove_cvref_t<M>>) {
                fn(bound_field<T, I(), const std::remove_cvref_t<M>>{member});
            }
        });
    }

    // ── Schema-only iteration — no instance needed ───────────────────

    template <typename F>
    constexpr void for_each(F&& fn) const {
        detail::for_each_field_schema<T>(
            std::forward<F>(fn),
            std::make_index_sequence<detail::dispatch_field_count<T>()>{}
        );
    }

    // ── Get field by runtime name ────────────────────────────────────

    template <typename F>
    constexpr bool get(T& obj, std::string_view name, F&& fn) const {
        bool found = false;
        for_each(obj, [&](auto field) {
            if (!found && field.name() == name) {
                fn(field);
                found = true;
            }
        });
        return found;
    }

    template <typename F>
    constexpr bool get(const T& obj, std::string_view name, F&& fn) const {
        bool found = false;
        for_each(obj, [&](auto field) {
            if (!found && field.name() == name) {
                fn(field);
                found = true;
            }
        });
        return found;
    }
};

// ── Entry points ────────────────────────────────────────────────────────

template <typename T>
constexpr auto reflect() {
    return reflection<T>{};
}

template <typename T>
constexpr auto reflect(const T&) {
    return reflection<T>{};
}

}  // namespace collab::field
