module;

#include <nlohmann/json.hpp>

#include <any>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <typeindex>
#include <typeinfo>

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

// ── type_def<dynamic_tag>::field(name, nested_type_def) ──────────────────

collab::model::type_def<collab::model::detail::dynamic_tag>&
collab::model::type_def<collab::model::detail::dynamic_tag>::field(
        std::string_view fname,
        const type_def& nested_type) {
    const auto* nested_ptr = &nested_type;

    auto setter = [](std::any& target, std::any&& incoming) -> bool {
        if (auto* p = std::any_cast<type_instance>(&incoming)) {
            target = std::move(*p);
            return true;
        }
        return false;
    };
    auto factory = [nested_ptr]() -> std::any {
        return std::any(nested_ptr->create());
    };

    // JSON codec — to_json calls type_instance::to_json(),
    // from_json creates a fresh instance and calls load_json().
    auto json_init = [nested_ptr](detail::dynamic_field_def& fd) {
        fd.to_json_fn = [](const std::any& a) -> std::any {
            const auto& instance = *std::any_cast<type_instance>(&a);
            return std::any(instance.to_json());
        };
        fd.from_json_fn = [nested_ptr](std::any& a, const std::any& j_any) {
            const auto& j = *std::any_cast<nlohmann::json>(&j_any);
            auto instance = nested_ptr->create();
            instance.load_json(j);
            a = std::move(instance);
        };
    };

    detail::dynamic_field_def fd{};
    fd.name = std::string(fname);
    fd.type = typeid(type_instance);
    fd.default_value = nested_type.create();
    fd.has_default = true;
    fd.setter = std::move(setter);
    fd.make_default = std::move(factory);
    fd._json_init = std::move(json_init);
    fields_.push_back(std::move(fd));
    return *this;
}
