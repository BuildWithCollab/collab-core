module;

#include <nlohmann/json.hpp>

#include <any>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

module collab.core;

// ── Lazy codec init ──────────────────────────────────────────────────────

static void ensure_codec(collab::model::detail::dynamic_field_def& fd) {
    if (fd.to_json_fn) return;
    if (fd._json_init) fd._json_init(fd);
}

// ── type_instance::load_json ─────────────────────────────────────────────

void collab::model::type_instance::load_json(const nlohmann::json& j) {
    if (!j.is_object())
        throw std::logic_error("load_json: expected JSON object");
    auto& fields = type_->fields_;
    for (auto& [key, val] : j.items()) {
        int idx = find_field_index(key);
        if (idx < 0) continue;
        auto& fd = const_cast<detail::dynamic_field_def&>(fields[idx]);
        ensure_codec(fd);
        fd.from_json_fn(values_[idx], std::any(val));
    }
}

// ── type_instance::to_json ───────────────────────────────────────────────

nlohmann::json collab::model::type_instance::to_json() const {
    nlohmann::json j = nlohmann::json::object();
    auto& fields = type_->fields_;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        auto& fd = const_cast<detail::dynamic_field_def&>(fields[i]);
        ensure_codec(fd);
        auto result = fd.to_json_fn(values_[i]);
        j[fd.name] = *std::any_cast<nlohmann::json>(&result);
    }
    return j;
}

// ── type_instance::to_json_string ────────────────────────────────────────

std::string collab::model::type_instance::to_json_string(int indent) const {
    return to_json().dump(indent);
}

// ── type_def<dynamic_tag>::create(json) ──────────────────────────────────

collab::model::type_instance
collab::model::type_def<collab::model::detail::dynamic_tag>::create(
        const nlohmann::json& j) const {
    auto obj = create();
    obj.load_json(j);
    return obj;
}
