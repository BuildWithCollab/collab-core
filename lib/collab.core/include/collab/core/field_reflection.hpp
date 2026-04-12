// 🔬 PFR-powered reflection utilities for Field<T>
//
// This lives as a header (not a module partition) because MSVC 14.44
// (VS 2022) ICEs when boost::pfr::fields_count is instantiated inside
// a module-exported template on structs containing module-exported
// template members. The bug is fixed in MSVC 14.50 (VS 2026).
//
// Field<T>, with<Exts...>, and IsField remain module-exported.
// Only the PFR-dependent code lives here.
#pragma once

#include <boost/pfr.hpp>
#include <nlohmann/json.hpp>

#include <ankerl/unordered_dense.h>

#include <cstddef>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace collab::core::fields {

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

// ── to_json ────────────────────────────────────────────────────────────
//
// Serializes a ReflectedStruct to JSON. Walks each member via PFR,
// keeps only IsField members, uses get_name for the key, unwraps
// .value, and hands it to nlohmann::json.
//
// Nested ReflectedStructs recurse. Non-Field members are skipped.
//
// Supported value types:
//   - Primitives: string, string_view, const char*, bool, int/int64/uint64, float/double
//   - Containers: vector<T>, set<T>, unordered_set<T>, optional<T>,
//                 map<string,V>, unordered_map<string,V>,
//                 ankerl::unordered_dense::map<string,V>,
//                 ankerl::unordered_dense::set<T>
//   - Nested ReflectedStructs

namespace detail {

    // ── Type traits for dispatching ──────────────────────────────────

    template <typename T>
    inline constexpr bool is_optional_v = false;

    template <typename T>
    inline constexpr bool is_optional_v<std::optional<T>> = true;

    template <typename T>
    inline constexpr bool is_iterable_array_v = false;

    template <typename T>
    inline constexpr bool is_iterable_array_v<std::vector<T>> = true;

    template <typename T, typename Cmp, typename Alloc>
    inline constexpr bool is_iterable_array_v<std::set<T, Cmp, Alloc>> = true;

    template <typename T, typename Hash, typename Eq, typename Alloc>
    inline constexpr bool is_iterable_array_v<std::unordered_set<T, Hash, Eq, Alloc>> = true;

    template <typename T, typename Hash, typename Eq>
    inline constexpr bool is_iterable_array_v<ankerl::unordered_dense::set<T, Hash, Eq>> = true;

    template <typename T>
    inline constexpr bool is_map_v = false;

    template <typename V, typename Cmp, typename Alloc>
    inline constexpr bool is_map_v<std::map<std::string, V, Cmp, Alloc>> = true;

    template <typename V, typename Hash, typename Eq, typename Alloc>
    inline constexpr bool is_map_v<std::unordered_map<std::string, V, Hash, Eq, Alloc>> = true;

    template <typename V, typename Hash, typename Eq>
    inline constexpr bool is_map_v<ankerl::unordered_dense::map<std::string, V, Hash, Eq>> = true;

    // ── Single dispatch point ────────────────────────────────────────

    template <typename T>
    nlohmann::json value_to_json(const T& v) {
        if constexpr (is_optional_v<T>) {
            if (v.has_value()) return value_to_json(*v);
            return nlohmann::json(nullptr);
        } else if constexpr (is_iterable_array_v<T>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& elem : v) arr.push_back(value_to_json(elem));
            return arr;
        } else if constexpr (is_map_v<T>) {
            nlohmann::json obj = nlohmann::json::object();
            for (const auto& [k, val] : v) {
                obj[k] = value_to_json(val);
            }
            return obj;
        } else if constexpr (ReflectedStruct<T>) {
            nlohmann::json j = nlohmann::json::object();
            boost::pfr::for_each_field(v, [&]<typename M>(const M& member, auto I) {
                if constexpr (IsField<M>) {
                    constexpr auto name = boost::pfr::get_name<I(), T>();
                    j[std::string(name)] = value_to_json(member.value);
                }
            });
            return j;
        } else {
            return nlohmann::json(v);
        }
    }

}  // namespace detail

// Returns a nlohmann::json object for programmatic use.
template <ReflectedStruct T>
nlohmann::json to_json(const T& obj) {
    return detail::value_to_json(obj);
}

// Returns a JSON string. indent < 0 → compact, indent >= 0 → pretty.
template <ReflectedStruct T>
std::string to_json_string(const T& obj, int indent = -1) {
    auto j = detail::value_to_json(obj);
    return indent < 0 ? j.dump() : j.dump(indent);
}

}  // namespace collab::core::fields
