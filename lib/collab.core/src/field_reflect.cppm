module;

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

export module collab.core:field_reflect;

import :field;
import :field_registry;
import :field_pfr;

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
    consteval auto collect_field_names(std::index_sequence<Is...>) {
        constexpr std::size_t total = sizeof...(Is);
        // First pass: count Field members
        constexpr std::size_t field_n = (0 + ... + (is_field<member_type<Is, T>> ? 1 : 0));
        // Second pass: collect their names
        std::array<std::string_view, field_n> result{};
        std::size_t idx = 0;
        ((is_field<member_type<Is, T>>
            ? (result[idx++] = dispatch_field_name<Is, T>(), 0) : 0), ...);
        return result;
    }

}  // namespace detail

template <typename T>
consteval auto field_names() {
    constexpr auto N = detail::dispatch_field_count<T>();
    return detail::collect_field_names<T>(std::make_index_sequence<N>{});
}

// ── field_descriptor<T, I> — schema-only field info ─────────────────────

template <typename T, std::size_t I>
struct field_descriptor {
    consteval std::string_view name() const {
        return detail::dispatch_field_name<I, T>();
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

    consteval std::string_view name() const {
        return detail::dispatch_field_name<I, T>();
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

    consteval auto names() const {
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
