module;

#include <nlohmann/json.hpp>
#include <magic_enum/magic_enum.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

module collab.core;


// ── type_instance::load_json — in-place overlay from JSON ────────────────
//
// Overlay semantics: missing keys keep defaults, extra keys ignored.
// Tries common types in priority order; the field's setter rejects mismatches.

void collab::model::type_instance::load_json(const nlohmann::json& j) {
    if (!j.is_object())
        throw std::logic_error("load_json: expected JSON object");
    for (auto& [key, val] : j.items()) {
        if (!has(key)) continue;
        bool set_ok = false;
        if (val.is_string()) {
            try { set(key, val.get<std::string>()); set_ok = true; }
            catch (const std::logic_error&) {}
        }
        if (!set_ok && val.is_boolean()) {
            try { set(key, val.get<bool>()); set_ok = true; }
            catch (const std::logic_error&) {}
        }
        if (!set_ok && val.is_number_integer()) {
            try { set(key, val.get<int>()); set_ok = true; }
            catch (const std::logic_error&) {}
            if (!set_ok) {
                try { set(key, val.get<int64_t>()); set_ok = true; }
                catch (const std::logic_error&) {}
            }
        }
        if (!set_ok && val.is_number_unsigned()) {
            try { set(key, val.get<uint64_t>()); set_ok = true; }
            catch (const std::logic_error&) {}
            if (!set_ok) {
                try { set(key, val.get<int>()); set_ok = true; }
                catch (const std::logic_error&) {}
            }
        }
        if (!set_ok && val.is_number_float()) {
            try { set(key, val.get<double>()); set_ok = true; }
            catch (const std::logic_error&) {}
            if (!set_ok) {
                try { set(key, val.get<float>()); set_ok = true; }
                catch (const std::logic_error&) {}
            }
        }
        if (!set_ok && val.is_null()) {
            continue;
        }
        if (!set_ok) {
            throw std::logic_error(
                "load_json: cannot deserialize field '" + key +
                "' — unsupported JSON type for dynamic type_def");
        }
    }
}

// ── type_instance::to_json — serialize fields to JSON object ─────────────
//
// Iterates fields and probes common types via try_as<T>().

nlohmann::json collab::model::type_instance::to_json() const {
    nlohmann::json j = nlohmann::json::object();
    for_each_field([&](std::string_view name, const_field_value value) {
        if (auto* p = value.try_as<std::string>())   { j[std::string(name)] = *p; return; }
        if (auto* p = value.try_as<bool>())           { j[std::string(name)] = *p; return; }
        if (auto* p = value.try_as<int>())            { j[std::string(name)] = *p; return; }
        if (auto* p = value.try_as<int64_t>())        { j[std::string(name)] = *p; return; }
        if (auto* p = value.try_as<uint64_t>())       { j[std::string(name)] = *p; return; }
        if (auto* p = value.try_as<double>())         { j[std::string(name)] = *p; return; }
        if (auto* p = value.try_as<float>())          { j[std::string(name)] = *p; return; }
    });
    return j;
}

// ── type_instance::to_json_string — dump JSON as string ──────────────────

std::string collab::model::type_instance::to_json_string(int indent) const {
    return to_json().dump(indent);
}

// ── type_def<dynamic_tag>::create(json) — factory from JSON ──────────────

collab::model::type_instance
collab::model::type_def<collab::model::detail::dynamic_tag>::create(
        const nlohmann::json& j) const {
    auto obj = create();
    obj.load_json(j);
    return obj;
}
