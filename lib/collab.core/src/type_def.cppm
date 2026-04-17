module;

#include <any>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <utility>
#include <vector>

export module collab.core:type_def;

import :field;
import :field_reflect;
import :meta;

export namespace collab::model {

// ── dynamic_tag — sentinel for the non-templated type_def ────────────────

struct dynamic_tag {};

class field_view;  // forward declaration for concept

// ── type_definition — concept satisfied by every type_def mode ───────────
//
// The constraint that says "this thing describes a type's shape."
// type_def<T>, type_def("Event"), and type_def<T>().field(...) all satisfy it.

namespace detail {
    struct concept_sentinel_meta {};
}

template <typename T>
concept type_definition = requires(const T& t, std::string_view sv) {
    // Schema queries
    { t.name() } -> std::convertible_to<std::string_view>;
    { t.field_count() } -> std::convertible_to<std::size_t>;
    { t.field_names() } -> std::same_as<std::vector<std::string>>;
    { t.has_field(sv) } -> std::same_as<bool>;
    // Field query by name
    { t.field(sv) } -> std::same_as<field_view>;
    // Meta queries
    { t.template has_meta<detail::concept_sentinel_meta>() } -> std::same_as<bool>;
    { t.template meta<detail::concept_sentinel_meta>() } -> std::same_as<detail::concept_sentinel_meta>;
    { t.template meta_count<detail::concept_sentinel_meta>() } -> std::convertible_to<std::size_t>;
    { t.template metas<detail::concept_sentinel_meta>() } -> std::same_as<std::vector<detail::concept_sentinel_meta>>;
};

// ── type_def<T> — typed runtime schema with auto-discovery ───────────────
//
// Automatically discovers field<> and meta<> members of T via PFR or
// the reflect_on<T>() registry. Provides runtime access to field values,
// type-level metadata, and schema information.
//
// Usage:
//   type_def<Dog> dog_t;
//   dog_t.name();                     // "Dog"
//   dog_t.field_count();              // 3
//   dog_t.has_meta<endpoint_info>();  // true
//   dog_t.meta<endpoint_info>().path; // "/dogs"
//   dog_t.for_each(rex, [](std::string_view name, auto& value) { ... });

namespace detail {

    // ── Discovery helpers ────────────────────────────────────────────

    // Count meta<M> members for a specific M
    template <typename T, typename M, std::size_t... Is>
    consteval std::size_t count_meta_of_impl(std::index_sequence<Is...>) {
        return (0 + ... + (is_meta_of_v<
            std::remove_cvref_t<collab::model::detail::member_type<Is, T>>, M> ? 1 : 0));
    }

    template <typename T, typename M>
    consteval std::size_t count_meta_of() {
        constexpr auto N = collab::model::detail::dispatch_field_count<T>();
        return count_meta_of_impl<T, M>(std::make_index_sequence<N>{});
    }

    // Count all meta<> members (any type)
    template <typename T, std::size_t... Is>
    consteval std::size_t count_all_metas_impl(std::index_sequence<Is...>) {
        return (0 + ... + (collab::model::is_meta<
            collab::model::detail::member_type<Is, T>> ? 1 : 0));
    }

    template <typename T>
    consteval std::size_t count_all_metas() {
        constexpr auto N = collab::model::detail::dispatch_field_count<T>();
        return count_all_metas_impl<T>(std::make_index_sequence<N>{});
    }

    // ── Meta extraction helpers ──────────────────────────────────────

    template <std::size_t I, typename Obj, typename M>
    constexpr void try_extract_meta(Obj& obj, M& result, bool& found) {
        if (found) return;
        using T = std::remove_cvref_t<Obj>;
        using member_t = std::remove_cvref_t<collab::model::detail::member_type<I, T>>;
        if constexpr (is_meta_of_v<member_t, M>) {
            result = collab::model::detail::dispatch_get_member<I>(obj).value;
            found = true;
        }
    }

    template <typename Obj, typename M, std::size_t... Is>
    constexpr M extract_first_meta(Obj& obj, std::index_sequence<Is...>) {
        M result{};
        bool found = false;
        (try_extract_meta<Is>(obj, result, found), ...);
        return result;
    }

    template <typename Obj, typename M, std::size_t... Is>
    constexpr std::vector<M> extract_all_metas(Obj& obj, std::index_sequence<Is...>) {
        std::vector<M> results;
        auto collect = [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
            using T = std::remove_cvref_t<Obj>;
            using member_t = std::remove_cvref_t<collab::model::detail::member_type<I, T>>;
            if constexpr (is_meta_of_v<member_t, M>) {
                results.push_back(
                    collab::model::detail::dispatch_get_member<I>(obj).value);
            }
        };
        (collect(std::integral_constant<std::size_t, Is>{}), ...);
        return results;
    }

    // ── Field iteration helpers ──────────────────────────────────────

    template <std::size_t I, typename Obj, typename F>
    constexpr void visit_field_value(Obj& obj, F&& fn) {
        using T = std::remove_cvref_t<Obj>;
        using member_t = collab::model::detail::member_type<I, T>;
        if constexpr (collab::model::is_field<member_t>) {
            auto& member = collab::model::detail::dispatch_get_member<I>(obj);
            fn(collab::model::detail::dispatch_field_name_rt<I, T>(), member.value);
        }
    }

    template <typename Obj, typename F, std::size_t... Is>
    constexpr void for_each_field_value(Obj& obj, F&& fn, std::index_sequence<Is...>) {
        (visit_field_value<Is>(obj, fn), ...);
    }

    // ── Schema-only field iteration ──────────────────────────────────

    template <typename T, std::size_t I, typename F>
    constexpr void visit_field_descriptor(F&& fn) {
        using member_t = collab::model::detail::member_type<I, T>;
        if constexpr (collab::model::is_field<member_t>) {
            fn(collab::model::field_descriptor<T, I>{});
        }
    }

    template <typename T, typename F, std::size_t... Is>
    constexpr void for_each_field_descriptor(F&& fn, std::index_sequence<Is...>) {
        (visit_field_descriptor<T, Is>(fn), ...);
    }

    // ── Meta iteration helpers ───────────────────────────────────────

    template <std::size_t I, typename Obj, typename F>
    constexpr void visit_meta_value(Obj& obj, F&& fn) {
        using T = std::remove_cvref_t<Obj>;
        using member_t = std::remove_cvref_t<collab::model::detail::member_type<I, T>>;
        if constexpr (collab::model::is_meta<member_t>) {
            fn(collab::model::detail::dispatch_get_member<I>(obj).value);
        }
    }

    template <typename Obj, typename F, std::size_t... Is>
    constexpr void for_each_meta_value(Obj& obj, F&& fn, std::index_sequence<Is...>) {
        (visit_meta_value<Is>(obj, fn), ...);
    }

    // ── Get by name ──────────────────────────────────────────────────

    template <std::size_t I, typename Obj, typename F>
    constexpr void try_get_field(Obj& obj, std::string_view name, F&& fn, bool& found) {
        if (found) return;
        using T = std::remove_cvref_t<Obj>;
        using member_t = collab::model::detail::member_type<I, T>;
        if constexpr (collab::model::is_field<member_t>) {
            if (collab::model::detail::dispatch_field_name_rt<I, T>() == name) {
                auto& member = collab::model::detail::dispatch_get_member<I>(obj);
                fn(collab::model::detail::dispatch_field_name_rt<I, T>(), member.value);
                found = true;
            }
        }
    }

    template <typename Obj, typename F, std::size_t... Is>
    constexpr bool get_field_by_name(Obj& obj, std::string_view name, F&& fn,
                                     std::index_sequence<Is...>) {
        bool found = false;
        (try_get_field<Is>(obj, name, fn, found), ...);
        return found;
    }

    // ── Set by name ──────────────────────────────────────────────────

    template <std::size_t I, typename Obj, typename V>
    constexpr void try_set_field(Obj& obj, std::string_view name, V&& val, bool& found) {
        if (found) return;
        using T = std::remove_cvref_t<Obj>;
        using member_t = collab::model::detail::member_type<I, T>;
        if constexpr (collab::model::is_field<member_t>) {
            if (collab::model::detail::dispatch_field_name_rt<I, T>() == name) {
                using value_t = typename member_t::value_type;
                if constexpr (std::is_constructible_v<value_t, V>) {
                    collab::model::detail::dispatch_get_member<I>(obj).value =
                        std::forward<V>(val);
                    found = true;
                }
            }
        }
    }

    template <typename Obj, typename V, std::size_t... Is>
    constexpr bool set_field_by_name(Obj& obj, std::string_view name, V&& val,
                                     std::index_sequence<Is...>) {
        bool found = false;
        (try_set_field<Is>(obj, name, std::forward<V>(val), found), ...);
        return found;
    }

    // ── Type-erased meta entry ───────────────────────────────────────

    struct meta_entry {
        std::type_index type{typeid(void)};
        std::any        value;
    };

    // ── Type-erased field definition (for dynamic type_def) ──────────

    struct dynamic_field_def {
        std::string              name;
        std::type_index          type{typeid(void)};
        std::any                 default_value;
        bool                     has_default = false;
        std::vector<meta_entry>  metas;

        std::function<bool(std::any&, std::any&&)> setter;
        std::function<std::any()> make_default;
    };

    // ── Extract metas from a with<Exts...> ───────────────────────────

    template <typename... Exts>
    void extract_with_metas(std::vector<meta_entry>& out, const with<Exts...>& w) {
        (out.push_back(
            {typeid(Exts), std::any(static_cast<const Exts&>(w))}), ...);
    }

    // ── Typed hybrid field registration (for type_def<T, Regs...>) ───
    //
    // Preserves the real member type MemT in the template parameter so
    // for_each can give the callback real typed references.

    template <typename T, typename MemT>
    struct typed_field_reg {
        MemT T::*               member;
        std::string             name;
        std::vector<meta_entry> metas;
    };

    template <typename T, typename MemT, typename... Withs>
    typed_field_reg<T, MemT> make_typed_reg(
            MemT T::* member, std::string_view fname, Withs... withs) {
        typed_field_reg<T, MemT> reg{member, std::string(fname), {}};
        (extract_with_metas(reg.metas, withs), ...);
        return reg;
    }

    // ── Build dynamic_field_def from auto-discovered field by name ──

    template <std::size_t I, typename TT>
    void try_build_discovered_field_def(
            dynamic_field_def& out, std::string_view target, bool& found) {
        if (found) return;
        using member_t = collab::model::detail::member_type<I, TT>;
        if constexpr (collab::model::is_field<member_t>) {
            if (collab::model::detail::dispatch_field_name_rt<I, TT>() == target) {
                out.name = std::string(target);
                out.type = typeid(typename member_t::value_type);
                out.has_default = false;
                out.metas.clear();
                found = true;
            }
        }
    }

    template <typename TT, std::size_t... Is>
    void build_discovered_field_def(
            dynamic_field_def& out, std::string_view target,
            bool& found, std::index_sequence<Is...>) {
        (try_build_discovered_field_def<Is, TT>(out, target, found), ...);
    }

}  // namespace detail

// ── field_view — read-only view into a dynamic field ─────────────

class field_view {
    const detail::dynamic_field_def* def_;

public:
    explicit field_view(const detail::dynamic_field_def* d) : def_(d) {}

    std::string_view name() const { return def_->name; }

    bool has_default() const { return def_->has_default; }

    template <typename V>
    V default_value() const { return *std::any_cast<V>(&def_->default_value); }

    template <typename M>
    bool has_meta() const {
        for (auto& e : def_->metas)
            if (e.type == typeid(M)) return true;
        return false;
    }

    template <typename M>
    M meta() const {
        for (auto& e : def_->metas)
            if (e.type == typeid(M))
                return *std::any_cast<M>(&e.value);
        return M{};
    }

    template <typename M>
    std::size_t meta_count() const {
        std::size_t n = 0;
        for (auto& e : def_->metas)
            if (e.type == typeid(M)) ++n;
        return n;
    }

    template <typename M>
    std::vector<M> metas() const {
        std::vector<M> result;
        for (auto& e : def_->metas)
            if (e.type == typeid(M))
                result.push_back(*std::any_cast<M>(&e.value));
        return result;
    }
};

// ── field_value — typed access to a type-erased value ─────────────────
//
// Wraps std::any internally but never exposes it. Users access values
// through .as<T>() or .try_as<T>().

class field_value {
    std::any* value_;
    friend class object;
    explicit field_value(std::any* v) : value_(v) {}

public:
    template <typename V>
    V& as() { return *std::any_cast<V>(value_); }

    template <typename V>
    const V& as() const { return *std::any_cast<V>(value_); }

    template <typename V>
    V* try_as() { return std::any_cast<V>(value_); }

    template <typename V>
    const V* try_as() const { return std::any_cast<V>(value_); }
};

class const_field_value {
    const std::any* value_;
    friend class object;
    explicit const_field_value(const std::any* v) : value_(v) {}

public:
    template <typename V>
    const V& as() const { return *std::any_cast<V>(value_); }

    template <typename V>
    const V* try_as() const { return std::any_cast<V>(value_); }
};

template <typename T = dynamic_tag, typename... Regs>
class type_def {
    static constexpr auto total_members_ = collab::model::detail::dispatch_field_count<T>();
    using indices_ = std::make_index_sequence<total_members_>;

    // Typed hybrid registrations — each Reg is a typed_field_reg<T, MemT>
    // preserving the real member type for real typed references in for_each.
    std::tuple<Regs...> typed_regs_;

    // Allow other type_def instantiations to access private constructor
    template <typename U, typename... Rs>
    friend class type_def;

    // Private constructor for builder chain
    explicit type_def(std::tuple<Regs...> regs) : typed_regs_(std::move(regs)) {}

public:
    type_def() = default;

    // ── Type name ────────────────────────────────────────────────────

    constexpr std::string_view name() const {
        return collab::model::type_name<T>();
    }

    // ── Field queries ────────────────────────────────────────────────

    std::size_t field_count() const {
        return collab::model::field_count<T>() + sizeof...(Regs);
    }

    std::vector<std::string> field_names() const {
        auto discovered = collab::model::field_names<T>();
        std::vector<std::string> result(discovered.begin(), discovered.end());
        std::apply([&](const auto&... regs) {
            (result.push_back(regs.name), ...);
        }, typed_regs_);
        return result;
    }

    // ── Field builder (hybrid registration) ──────────────────────────
    //
    // Returns a NEW type_def with the member type preserved in the
    // template parameters. Each .field() call produces a new type.

    template <typename MemT, typename... Withs>
    auto field(MemT T::* member, std::string_view fname, Withs... withs) {
        auto new_reg = detail::make_typed_reg<T>(member, fname, withs...);
        auto new_tuple = std::tuple_cat(
            std::move(typed_regs_),
            std::make_tuple(std::move(new_reg)));
        return type_def<T, Regs..., detail::typed_field_reg<T, MemT>>(
            std::move(new_tuple));
    }

    // ── Field queries by name ────────────────────────────────────────

    bool has_field(std::string_view fname) const {
        auto discovered = collab::model::field_names<T>();
        for (auto& n : discovered)
            if (n == fname) return true;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && (found = true, true)), ...);
        }, typed_regs_);
        return found;
    }

    field_view field(std::string_view fname) const {
        // Check hybrid registered fields
        thread_local detail::dynamic_field_def temp;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !found && (
                temp.name = regs.name,
                temp.type = typeid(std::remove_reference_t<
                    decltype(std::declval<T>().*(regs.member))>),
                temp.has_default = false,
                temp.metas = regs.metas,
                found = true, true)), ...);
        }, typed_regs_);
        if (found) return field_view(&temp);
        // Check auto-discovered field<> members
        thread_local detail::dynamic_field_def discovered;
        found = false;
        detail::build_discovered_field_def<T>(
            discovered, fname, found, indices_{});
        if (found) return field_view(&discovered);
        // Not found
        thread_local detail::dynamic_field_def not_found;
        not_found = {};
        not_found.name = std::string(fname);
        return field_view(&not_found);
    }

    // ── Meta queries ─────────────────────────────────────────────────

    template <typename M>
    constexpr bool has_meta() const {
        return detail::count_meta_of<T, M>() > 0;
    }

    template <typename M>
    M meta() const {
        const T instance{};
        return detail::extract_first_meta<const T, M>(instance, indices_{});
    }

    template <typename M>
    constexpr std::size_t meta_count() const {
        return detail::count_meta_of<T, M>();
    }

    template <typename M>
    std::vector<M> metas() const {
        const T instance{};
        return detail::extract_all_metas<const T, M>(instance, indices_{});
    }

    // ── Instance iteration ──────────────────────────────────────────
    //
    // Callback signature: fn(std::string_view name, auto& value)
    // Auto-discovered field<> members give typed references.
    // Hybrid-registered fields also give real typed references.

    template <typename F>
    void for_each(T& obj, F&& fn) const {
        detail::for_each_field_value(obj, std::forward<F>(fn), indices_{});
        std::apply([&](const auto&... regs) {
            (fn(std::string_view(regs.name), obj.*(regs.member)), ...);
        }, typed_regs_);
    }

    template <typename F>
    void for_each(const T& obj, F&& fn) const {
        detail::for_each_field_value(obj, std::forward<F>(fn), indices_{});
        std::apply([&](const auto&... regs) {
            (fn(std::string_view(regs.name), obj.*(regs.member)), ...);
        }, typed_regs_);
    }

    // ── Schema-only field iteration ──────────────────────────────────
    //
    // Auto-discovered field<> members: callback receives field_descriptor<T, I>.
    // Hybrid-registered fields: callback receives field_view.
    // Both expose .name() and .has_meta<M>() / .meta<M>().

    template <typename F>
    void for_each_field(F&& fn) const {
        detail::for_each_field_descriptor<T>(std::forward<F>(fn), indices_{});
        std::apply([&](const auto&... regs) {
            ((void)[&] {
                detail::dynamic_field_def temp{
                    regs.name,
                    typeid(std::remove_reference_t<
                        decltype(std::declval<T>().*(regs.member))>),
                    {}, false, regs.metas, {}, {}};
                fn(field_view(&temp));
            }(), ...);
        }, typed_regs_);
    }

    // ── Meta iteration ───────────────────────────────────────────────

    template <typename F>
    void for_each_meta(F&& fn) const {
        const T instance{};
        detail::for_each_meta_value(instance, std::forward<F>(fn), indices_{});
    }

    template <typename F>
    constexpr void for_each_meta(const T& obj, F&& fn) const {
        detail::for_each_meta_value(obj, std::forward<F>(fn), indices_{});
    }

    // ── Get field by runtime name (callback) ────────────────────────

    template <typename F>
    bool get(T& obj, std::string_view fname, F&& fn) const {
        if (detail::get_field_by_name(obj, fname, std::forward<F>(fn), indices_{}))
            return true;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !found &&
                (fn(std::string_view(regs.name), obj.*(regs.member)),
                 found = true, true)), ...);
        }, typed_regs_);
        return found;
    }

    template <typename F>
    bool get(const T& obj, std::string_view fname, F&& fn) const {
        if (detail::get_field_by_name(obj, fname, std::forward<F>(fn), indices_{}))
            return true;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !found &&
                (fn(std::string_view(regs.name), obj.*(regs.member)),
                 found = true, true)), ...);
        }, typed_regs_);
        return found;
    }

    // ── Get field by runtime name (typed) ────────────────────────────

    template <typename V>
    std::optional<V> get(const T& obj, std::string_view fname) const {
        std::optional<V> result;
        detail::get_field_by_name(obj, fname,
            [&](std::string_view, const auto& value) {
                if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, V>)
                    result = value;
            }, indices_{});
        if (result) return result;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !result.has_value() && [&] {
                using MemT = std::remove_reference_t<
                    decltype(std::declval<T>().*(regs.member))>;
                if constexpr (std::is_same_v<MemT, V>)
                    result = obj.*(regs.member);
                return true;
            }()), ...);
        }, typed_regs_);
        return result;
    }

    // ── Set field by runtime name ────────────────────────────────────

    template <typename V>
    bool set(T& obj, std::string_view fname, V&& val) const {
        if (detail::set_field_by_name(
                obj, fname, std::forward<V>(val), indices_{}))
            return true;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !found && [&] {
                using MemT = std::remove_reference_t<
                    decltype(std::declval<T>().*(regs.member))>;
                if constexpr (std::is_constructible_v<MemT, V>) {
                    obj.*(regs.member) = MemT(std::forward<V>(val));
                    found = true;
                }
                return true;
            }()), ...);
        }, typed_regs_);
        return found;
    }

    // ── Create instance ──────────────────────────────────────────────

    T create() const { return T{}; }
};

// ═══════════════════════════════════════════════════════════════════════
// Dynamic type_def — runtime builder, no backing struct
// ═══════════════════════════════════════════════════════════════════════

// Forward declaration for object (defined below)
class object;

// ── type_def<dynamic_tag> — the non-templated dynamic type_def ───────────

template <>
class type_def<dynamic_tag> {
    friend class object;

    std::string                           name_;
    std::vector<detail::dynamic_field_def> fields_;
    std::vector<detail::meta_entry>        type_metas_;

public:
    explicit type_def(std::string_view name) : name_(name) {}

    // ── Schema queries ───────────────────────────────────────────────

    std::string_view name() const { return name_; }

    std::size_t field_count() const { return fields_.size(); }

    std::vector<std::string> field_names() const {
        std::vector<std::string> result;
        result.reserve(fields_.size());
        for (auto& f : fields_) result.push_back(f.name);
        return result;
    }

    // ── Field builder ────────────────────────────────────────────────

    template <typename V>
    type_def& field(std::string_view fname) {
        auto setter = [](std::any& target, std::any&& incoming) -> bool {
            if (auto* p = std::any_cast<V>(&incoming)) {
                target = std::move(*p);
                return true;
            }
            return false;
        };
        auto factory = []() -> std::any { return std::any(V{}); };
        fields_.push_back({std::string(fname), typeid(V), {}, false, {},
                           std::move(setter), std::move(factory)});
        return *this;
    }

    template <typename V, typename... Withs>
    type_def& field(std::string_view fname, V default_value, Withs... withs) {
        auto setter = [](std::any& target, std::any&& incoming) -> bool {
            if (auto* p = std::any_cast<V>(&incoming)) {
                target = std::move(*p);
                return true;
            }
            return false;
        };
        auto factory = []() -> std::any { return std::any(V{}); };
        detail::dynamic_field_def fd{
            std::string(fname), typeid(V),
            std::any(std::move(default_value)), true, {},
            std::move(setter), std::move(factory)};
        (detail::extract_with_metas(fd.metas, withs), ...);
        fields_.push_back(std::move(fd));
        return *this;
    }

    // ── Meta builder (type-level) ────────────────────────────────────

    template <typename M>
    type_def& meta(M value) {
        type_metas_.push_back({typeid(M), std::any(std::move(value))});
        return *this;
    }

    // ── Field queries ────────────────────────────────────────────────

    bool has_field(std::string_view fname) const {
        for (auto& f : fields_)
            if (f.name == fname) return true;
        return false;
    }

    field_view field(std::string_view fname) const {
        for (auto& f : fields_)
            if (f.name == fname) return field_view(&f);
        // Return a view to the first field as fallback — caller should
        // check has_field() first. A real implementation might throw.
        return field_view(&fields_.front());
    }

    // ── Field iteration (schema-only) ────────────────────────────────

    template <typename F>
    void for_each_field(F&& fn) const {
        for (auto& f : fields_)
            fn(field_view(&f));
    }

    // ── Type-level meta queries ──────────────────────────────────────

    template <typename M>
    bool has_meta() const {
        for (auto& e : type_metas_)
            if (e.type == typeid(M)) return true;
        return false;
    }

    template <typename M>
    M meta() const {
        for (auto& e : type_metas_)
            if (e.type == typeid(M))
                return *std::any_cast<M>(&e.value);
        return M{};
    }

    template <typename M>
    std::size_t meta_count() const {
        std::size_t n = 0;
        for (auto& e : type_metas_)
            if (e.type == typeid(M)) ++n;
        return n;
    }

    template <typename M>
    std::vector<M> metas() const {
        std::vector<M> result;
        for (auto& e : type_metas_)
            if (e.type == typeid(M))
                result.push_back(*std::any_cast<M>(&e.value));
        return result;
    }

    // ── Create instance ─────────────────────────────────────────────

    object create() const;
};

// ── CTAD: type_def("Event") deduces to type_def<dynamic_tag> ─────────────

type_def(const char*) -> type_def<dynamic_tag>;
type_def(std::string_view) -> type_def<dynamic_tag>;

// ═══════════════════════════════════════════════════════════════════════
// object — instance of a dynamic type_def
// ═══════════════════════════════════════════════════════════════════════

class object {
    const type_def<dynamic_tag>* type_;
    std::vector<std::any>        values_;

    int find_field_index(std::string_view name) const {
        auto& fields = type_->fields_;
        for (int i = 0; i < static_cast<int>(fields.size()); ++i)
            if (fields[i].name == name) return i;
        return -1;
    }

public:
    explicit object(const type_def<dynamic_tag>& t) : type_(&t) {
        values_.reserve(t.fields_.size());
        for (auto& fd : t.fields_) {
            if (fd.has_default)
                values_.push_back(fd.default_value);
            else
                values_.push_back(fd.make_default());
        }
    }

    // ── Set ──────────────────────────────────────────────────────────

    template <typename V>
    bool set(std::string_view name, V&& value) {
        int idx = find_field_index(name);
        if (idx < 0) return false;
        auto& fd = type_->fields_[idx];
        std::any wrapped(std::forward<V>(value));
        if (fd.setter(values_[idx], std::move(wrapped))) return true;
        // Fallback: try string conversion for string-like types
        if constexpr (std::is_constructible_v<std::string, V> &&
                      !std::is_same_v<std::remove_cvref_t<V>, std::string>) {
            std::any str(std::string(std::forward<V>(value)));
            if (fd.setter(values_[idx], std::move(str))) return true;
        }
        return false;
    }

    // ── Get ──────────────────────────────────────────────────────────

    template <typename V>
    std::optional<V> get(std::string_view name) const {
        int idx = find_field_index(name);
        if (idx < 0) return std::nullopt;
        if (auto* p = std::any_cast<V>(&values_[idx]))
            return *p;
        return std::nullopt;
    }

    // ── Has ──────────────────────────────────────────────────────────

    bool has(std::string_view name) const {
        return find_field_index(name) >= 0;
    }

    // ── Type access ──────────────────────────────────────────────────

    const type_def<dynamic_tag>& type() const { return *type_; }

    // ── Iteration ────────────────────────────────────────────────────
    //
    // Callback receives (std::string_view name, const_field_value value)
    // or (std::string_view name, field_value value).
    // Access typed values via value.as<T>() or value.try_as<T>().

    template <typename F>
    void for_each(F&& fn) const {
        auto& fields = type_->fields_;
        for (std::size_t i = 0; i < fields.size(); ++i)
            fn(std::string_view(fields[i].name),
               const_field_value(&values_[i]));
    }

    template <typename F>
    void for_each(F&& fn) {
        auto& fields = type_->fields_;
        for (std::size_t i = 0; i < fields.size(); ++i)
            fn(std::string_view(fields[i].name),
               field_value(&values_[i]));
    }
};

// ── type_def<dynamic_tag>::create() ──────────────────────────────────────

inline object type_def<dynamic_tag>::create() const {
    return object(*this);
}

}  // namespace collab::model
