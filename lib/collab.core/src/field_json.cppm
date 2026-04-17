module;

#include <nlohmann/json.hpp>

#include <string>

export module collab.core:field_json;

import :field;
import :field_reflect;
import :meta;
import :type_def;

export namespace collab::model {

// ── to_json ────────────────────────────────────────────────────────────

template <detail::reflected_struct T>
nlohmann::json to_json(const T& obj) {
    return detail::value_to_json(obj);
}

template <detail::reflected_struct T>
std::string to_json_string(const T& obj, int indent = -1) {
    auto j = detail::value_to_json(obj);
    return indent < 0 ? j.dump() : j.dump(indent);
}

// ── from_json ──────────────────────────────────────────────────────────
//
// Deserializes JSON into a reflected_struct. Overlay semantics:
// - Missing keys leave the field at its default value.
// - Extra JSON keys are silently ignored.
// - Type mismatches throw std::logic_error.
// - Nested reflected_structs recurse automatically.
// - meta<> and non-field<> members are untouched.

template <detail::reflected_struct T>
T from_json(const nlohmann::json& j) {
    T result{};
    detail::value_from_json(j, result);
    return result;
}

template <detail::reflected_struct T>
T from_json(const std::string& json_str) {
    auto j = nlohmann::json::parse(json_str);
    return from_json<T>(j);
}

// ── Dynamic JSON deserialization ─────────────────────────────────────────
//
// type_instance::load_json and type_def::create(json) are defined in
// field_json.cpp (module implementation unit) to avoid MSVC linker issues
// with inline cross-partition member definitions.

}  // namespace collab::model
