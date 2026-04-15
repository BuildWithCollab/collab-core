module;

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

export module collab.core:field_json;

import :field;
import :field_reflect;

export namespace collab::field {

// ── to_json ────────────────────────────────────────────────────────────
//
// Serializes a reflected_struct to JSON via reflect<T>().for_each().
// Only Field<> members are visited; non-Field members are skipped.
// Nested reflected_structs recurse automatically.

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
    //
    // if constexpr handles all cases without overload resolution issues.

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
        } else if constexpr (reflected_struct<T>) {
            nlohmann::json j = nlohmann::json::object();
            reflect<T>().for_each(v, [&](auto field) {
                j[std::string(field.name())] = value_to_json(field.value());
            });
            return j;
        } else {
            return nlohmann::json(v);
        }
    }

}  // namespace detail

// Returns a nlohmann::json object for programmatic use.
template <reflected_struct T>
nlohmann::json to_json(const T& obj) {
    return detail::value_to_json(obj);
}

// Returns a JSON string. indent < 0 → compact, indent >= 0 → pretty.
template <reflected_struct T>
std::string to_json_string(const T& obj, int indent = -1) {
    auto j = detail::value_to_json(obj);
    return indent < 0 ? j.dump() : j.dump(indent);
}

}  // namespace collab::field
