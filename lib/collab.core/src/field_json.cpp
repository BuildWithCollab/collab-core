module;

#include <nlohmann/json.hpp>

#include <any>
#include <stdexcept>
#include <string>
#include <string_view>

module collab.core;


// ── type_instance::load_json — in-place overlay from JSON ────────────────
//
// Overlay semantics: missing keys keep defaults, extra keys ignored.
// Uses the captured from_json_fn lambda from field registration time.
// The lambda expects nlohmann::json wrapped in std::any.

void collab::model::type_instance::load_json(const nlohmann::json& j) {
    if (!j.is_object())
        throw std::logic_error("load_json: expected JSON object");
    auto& fields = type_->fields_;
    for (auto& [key, val] : j.items()) {
        int idx = find_field_index(key);
        if (idx < 0) continue;  // extra keys silently ignored
        auto& fd = fields[idx];
        fd.from_json_fn(values_[idx], std::any(val));
    }
}

// ── type_instance::to_json — serialize fields to JSON object ─────────────
//
// Uses the captured to_json_fn lambda from field registration time.
// The lambda returns nlohmann::json wrapped in std::any.

nlohmann::json collab::model::type_instance::to_json() const {
    nlohmann::json j = nlohmann::json::object();
    auto& fields = type_->fields_;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        auto result = fields[i].to_json_fn(values_[i]);
        j[fields[i].name] = *std::any_cast<nlohmann::json>(&result);
    }
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
