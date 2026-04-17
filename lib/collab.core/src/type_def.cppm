module;

#include <any>
#include <cstddef>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include <ankerl/unordered_dense.h>

#include <magic_enum/magic_enum.hpp>

export module collab.core:type_def;

import :field;
import :field_reflect;
import :meta;

export namespace collab::model {

namespace detail {
    // Sentinel type for the non-templated type_def. Users never see this —
    // CTAD deduces type_def("Event") → type_def<detail::dynamic_tag>.
    struct dynamic_tag {};
}  // namespace detail

class field_def;  // forward declaration for concept

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
    { t.field(sv) } -> std::same_as<field_def>;
    // Meta queries
    { t.template has_meta<detail::concept_sentinel_meta>() } -> std::same_as<bool>;
    { t.template meta<detail::concept_sentinel_meta>() } -> std::same_as<detail::concept_sentinel_meta>;
    { t.template meta_count<detail::concept_sentinel_meta>() } -> std::convertible_to<std::size_t>;
    { t.template metas<detail::concept_sentinel_meta>() } -> std::same_as<std::vector<detail::concept_sentinel_meta>>;
    // Schema iteration
    t.for_each_field([](auto) {});
    t.for_each_meta([](auto) {});
    // Create instance
    t.create();
};

// ── type_def<T> — typed runtime schema with auto-discovery ───────────────
//
// Automatically discovers field<> and meta<> members of T via PFR or
// the struct_info<T>() registry. Provides runtime access to field values,
// type-level metadata, and schema information.
//
// Usage:
//   type_def<Dog> dog_t;
//   dog_t.name();                     // "Dog"
//   dog_t.field_count();              // 3
//   dog_t.has_meta<endpoint_info>();  // true
//   dog_t.meta<endpoint_info>().path; // "/dogs"
//   dog_t.for_each_field(rex, [](std::string_view name, auto& value) { ... });

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
        // field_indices_ guarantees only field<> member indices arrive here
        auto& member = collab::model::detail::dispatch_get_member<I>(obj);
        if (collab::model::detail::dispatch_field_name_rt<I, T>() == name) {
            fn(collab::model::detail::dispatch_field_name_rt<I, T>(), member.value);
            found = true;
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
    constexpr void try_set_field(Obj& obj, std::string_view name, V&& val,
                                 bool& set_ok, bool& name_matched) {
        if (set_ok) return;
        using T = std::remove_cvref_t<Obj>;
        using member_t = collab::model::detail::member_type<I, T>;
        // field_indices_ guarantees only field<> member indices arrive here
        if (collab::model::detail::dispatch_field_name_rt<I, T>() == name) {
            name_matched = true;
            using value_t = typename member_t::value_type;
            if constexpr (std::is_constructible_v<value_t, V>) {
                collab::model::detail::dispatch_get_member<I>(obj).value =
                    std::forward<V>(val);
                set_ok = true;
            }
        }
    }

    template <typename Obj, typename V, std::size_t... Is>
    constexpr void set_field_by_name(Obj& obj, std::string_view name, V&& val,
                                     std::index_sequence<Is...>,
                                     bool& name_matched, bool& set_ok) {
        (try_set_field<Is>(obj, name, std::forward<V>(val), set_ok, name_matched), ...);
    }

    // ── JSON value helpers (used by dynamic field lambdas) ──────────

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

    template <typename T> struct array_element;
    template <typename T> struct array_element<std::vector<T>> { using type = T; };
    template <typename T, typename Cmp, typename Alloc>
    struct array_element<std::set<T, Cmp, Alloc>> { using type = T; };
    template <typename T, typename Hash, typename Eq, typename Alloc>
    struct array_element<std::unordered_set<T, Hash, Eq, Alloc>> { using type = T; };
    template <typename T, typename Hash, typename Eq>
    struct array_element<ankerl::unordered_dense::set<T, Hash, Eq>> { using type = T; };
    template <typename T>
    using array_element_t = typename array_element<T>::type;

    template <typename T> struct map_value;
    template <typename V, typename Cmp, typename Alloc>
    struct map_value<std::map<std::string, V, Cmp, Alloc>> { using type = V; };
    template <typename V, typename Hash, typename Eq, typename Alloc>
    struct map_value<std::unordered_map<std::string, V, Hash, Eq, Alloc>> { using type = V; };
    template <typename V, typename Hash, typename Eq>
    struct map_value<ankerl::unordered_dense::map<std::string, V, Hash, Eq>> { using type = V; };
    template <typename T>
    using map_value_t = typename map_value<T>::type;

    template <typename T> struct optional_inner;
    template <typename T> struct optional_inner<std::optional<T>> { using type = T; };
    template <typename T>
    using optional_inner_t = typename optional_inner<T>::type;

    // Forward declare — defined after type_def<T> (needs for_each_field)
    template <typename T>
    nlohmann::json value_to_json(const T& v);

    template <typename T>
    void value_from_json(const nlohmann::json& j, T& out);

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

        // JSON serialization — captured at .field<V>() time
        std::function<nlohmann::json(const std::any&)> to_json_fn;
        std::function<void(std::any&, const nlohmann::json&)> from_json_fn;
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
                TT instance{};
                extract_with_metas(out.metas,
                    collab::model::detail::dispatch_get_member<I>(instance).with);
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

// ── field_def — read-only view into a dynamic field ─────────────

class field_def {
    const detail::dynamic_field_def* def_;

public:
    explicit field_def(const detail::dynamic_field_def* d) : def_(d) {}

    std::string_view name() const { return def_->name; }

    bool has_default() const { return def_->has_default; }

    template <typename V>
    V default_value() const {
        auto* p = std::any_cast<V>(&def_->default_value);
        if (!p) throw std::logic_error(
            "field '" + std::string(name()) + "': default_value type mismatch");
        return *p;
    }

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
        throw std::logic_error(
            "field '" + std::string(name()) + "': no meta of requested type");
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
            if (e.type == typeid(M)) {
                auto* p = std::any_cast<M>(&e.value);
                if (!p) throw std::logic_error(
                    "field '" + std::string(name()) + "': meta storage corrupted");
                result.push_back(*p);
            }
        return result;
    }
};

// ── metadata — typed access to a type-erased meta ────────────────────
//
// Wraps std::any internally but never exposes it. Users access the
// underlying meta struct through .as<M>(), .try_as<M>(), or .is<M>().
// Metas are always read-only — they belong to the schema, not the instance.

class metadata {
    const std::any* value_;
    std::type_index type_;

public:
    // Constructor is public but useless without raw pointers to internals.
    // Only type_def<detail::dynamic_tag> has those.
    explicit metadata(const std::any* v, const std::type_index& t)
        : value_(v), type_(t) {}

    template <typename M>
    const M& as() const {
        auto* p = std::any_cast<M>(value_);
        if (!p) throw std::logic_error("metadata: type mismatch in as<>()");
        return *p;
    }

    template <typename M>
    const M* try_as() const { return std::any_cast<M>(value_); }

    template <typename M>
    bool is() const { return type_ == typeid(M); }
};

// ── field_value — typed access to a type-erased value ─────────────────
//
// Wraps std::any internally but never exposes it. Users access values
// through .as<T>() or .try_as<T>().

class field_value {
    std::any* value_;
    friend class type_instance;
    explicit field_value(std::any* v) : value_(v) {}

public:
    template <typename V>
    V& as() {
        auto* p = std::any_cast<V>(value_);
        if (!p) throw std::logic_error("field_value: type mismatch in as<>()");
        return *p;
    }

    template <typename V>
    const V& as() const {
        auto* p = std::any_cast<V>(value_);
        if (!p) throw std::logic_error("field_value: type mismatch in as<>()");
        return *p;
    }

    template <typename V>
    V* try_as() { return std::any_cast<V>(value_); }

    template <typename V>
    const V* try_as() const { return std::any_cast<V>(value_); }
};

class const_field_value {
    const std::any* value_;
    friend class type_instance;
    explicit const_field_value(const std::any* v) : value_(v) {}

public:
    template <typename V>
    const V& as() const {
        auto* p = std::any_cast<V>(value_);
        if (!p) throw std::logic_error("const_field_value: type mismatch in as<>()");
        return *p;
    }

    template <typename V>
    const V* try_as() const { return std::any_cast<V>(value_); }
};

namespace detail {

// ── Compile-time index filter: keep only indices where Pred<I, T> is true ──
template <typename T, template<std::size_t, typename> class Pred, std::size_t... Is>
consteval auto filter_indices(std::index_sequence<Is...>) {
    constexpr auto N = (... + (Pred<Is, T>::value ? 1 : 0));
    std::array<std::size_t, N> arr{};
    std::size_t pos = 0;
    ((Pred<Is, T>::value ? (arr[pos++] = Is, 0) : 0), ...);
    return arr;
}

template <typename T, std::size_t... Is>
consteval auto array_to_index_seq(std::array<std::size_t, sizeof...(Is)>, std::index_sequence<Is...>);

template <typename T, template<std::size_t, typename> class Pred, std::size_t... AllIs>
auto make_filtered_sequence(std::index_sequence<AllIs...>) {
    constexpr auto arr = filter_indices<T, Pred>(std::index_sequence<AllIs...>{});
    return [&]<std::size_t... Js>(std::index_sequence<Js...>) {
        return std::index_sequence<arr[Js]...>{};
    }(std::make_index_sequence<arr.size()>{});
}

template <std::size_t I, typename T>
struct is_field_at : std::bool_constant<collab::model::is_field<
    collab::model::detail::member_type<I, T>>> {};

}  // namespace detail

template <typename T = detail::dynamic_tag, typename... Regs>
class type_def {
    static constexpr auto total_members_ = collab::model::detail::dispatch_field_count<T>();
    using indices_ = std::make_index_sequence<total_members_>;
    using field_indices_ = decltype(detail::make_filtered_sequence<T, detail::is_field_at>(indices_{}));

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
        return collab::model::detail::type_name<T>();
    }

    // ── Field queries ────────────────────────────────────────────────

    std::size_t field_count() const {
        return collab::model::detail::field_count<T>() + sizeof...(Regs);
    }

    std::vector<std::string> field_names() const {
        auto discovered = collab::model::detail::field_names<T>();
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
        auto discovered = collab::model::detail::field_names<T>();
        for (auto& n : discovered)
            if (n == fname) return true;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && (found = true, true)), ...);
        }, typed_regs_);
        return found;
    }

    field_def field(std::string_view fname) const {
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
        if (found) return field_def(&temp);
        // Check auto-discovered field<> members
        thread_local detail::dynamic_field_def discovered;
        found = false;
        detail::build_discovered_field_def<T>(
            discovered, fname, found, indices_{});
        if (found) return field_def(&discovered);
        throw std::logic_error(
            "type_def '" + std::string(name()) + "': no field named '" +
            std::string(fname) + "'");
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

    // ── Instance field iteration ────────────────────────────────────
    //
    // Callback signature: fn(std::string_view name, auto& value)
    // Auto-discovered field<> members give typed references.
    // Hybrid-registered fields also give real typed references.

    template <typename F>
    void for_each_field(T& obj, F&& fn) const {
        detail::for_each_field_value(obj, std::forward<F>(fn), indices_{});
        std::apply([&](const auto&... regs) {
            (fn(std::string_view(regs.name), obj.*(regs.member)), ...);
        }, typed_regs_);
    }

    template <typename F>
    void for_each_field(const T& obj, F&& fn) const {
        detail::for_each_field_value(obj, std::forward<F>(fn), indices_{});
        std::apply([&](const auto&... regs) {
            (fn(std::string_view(regs.name), obj.*(regs.member)), ...);
        }, typed_regs_);
    }

    // ── Schema-only field iteration ──────────────────────────────────
    //
    // Auto-discovered field<> members: callback receives field_descriptor<T, I>.
    // Hybrid-registered fields: callback receives field_def.
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
                fn(field_def(&temp));
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
    void get(T& obj, std::string_view fname, F&& fn) const {
        if (detail::get_field_by_name(obj, fname, std::forward<F>(fn), field_indices_{}))
            return;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !found &&
                (fn(std::string_view(regs.name), obj.*(regs.member)),
                 found = true, true)), ...);
        }, typed_regs_);
        if (!found)
            throw std::logic_error(
                "type_def '" + std::string(name()) + "': no field named '" +
                std::string(fname) + "'");
    }

    template <typename F>
    void get(const T& obj, std::string_view fname, F&& fn) const {
        if (!has_field(fname)) {
            bool is_hybrid = false;
            std::apply([&](const auto&... regs) {
                ((regs.name == fname && (is_hybrid = true, true)), ...);
            }, typed_regs_);
            if (!is_hybrid)
                throw std::logic_error(
                    "type_def '" + std::string(name()) + "': no field named '" +
                    std::string(fname) + "'");
        }
        if (detail::get_field_by_name(obj, fname, std::forward<F>(fn), field_indices_{}))
            return;
        bool found = false;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !found &&
                (fn(std::string_view(regs.name), obj.*(regs.member)),
                 found = true, true)), ...);
        }, typed_regs_);
        if (!found)
            throw std::logic_error(
                "type_def '" + std::string(name()) + "': no field named '" +
                std::string(fname) + "'");
    }

    // ── Get field by runtime name (typed) ────────────────────────────

    template <typename V>
    V get(const T& obj, std::string_view fname) const {
        std::optional<V> result;
        bool field_found = detail::get_field_by_name(obj, fname,
            [&](std::string_view, const auto& value) {
                if constexpr (std::is_same_v<std::remove_cvref_t<decltype(value)>, V>)
                    result = value;
            }, field_indices_{});
        if (result) return *result;
        bool name_found = field_found;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !result.has_value() && [&] {
                name_found = true;
                using MemT = std::remove_reference_t<
                    decltype(std::declval<T>().*(regs.member))>;
                if constexpr (std::is_same_v<MemT, V>)
                    result = obj.*(regs.member);
                return true;
            }()), ...);
        }, typed_regs_);
        if (result) return *result;
        if (name_found)
            throw std::logic_error(
                "type_def '" + std::string(name()) + "': field '" +
                std::string(fname) + "' type mismatch");
        throw std::logic_error(
            "type_def '" + std::string(name()) + "': no field named '" +
            std::string(fname) + "'");
    }

    // ── Set field by runtime name ────────────────────────────────────

    template <typename V>
    void set(T& obj, std::string_view fname, V&& val) const {
        bool name_matched = false;
        bool set_ok = false;
        detail::set_field_by_name(
            obj, fname, std::forward<V>(val), field_indices_{},
            name_matched, set_ok);
        if (set_ok) return;
        std::apply([&](const auto&... regs) {
            ((regs.name == fname && !set_ok && [&] {
                name_matched = true;
                using MemT = std::remove_reference_t<
                    decltype(std::declval<T>().*(regs.member))>;
                if constexpr (std::is_constructible_v<MemT, V>) {
                    obj.*(regs.member) = MemT(std::forward<V>(val));
                    set_ok = true;
                }
                return true;
            }()), ...);
        }, typed_regs_);
        if (set_ok) return;
        if (name_matched)
            throw std::logic_error(
                "type_def '" + std::string(name()) + "': field '" +
                std::string(fname) + "' type mismatch");
        throw std::logic_error(
            "type_def '" + std::string(name()) + "': no field named '" +
            std::string(fname) + "'");
    }

    // ── Create instance ──────────────────────────────────────────────

    T create() const { return T{}; }
};

// ═══════════════════════════════════════════════════════════════════════
// Dynamic type_def — runtime builder, no backing struct
// ═══════════════════════════════════════════════════════════════════════

// Forward declaration for type_instance (defined below)
class type_instance;

// ── type_def<detail::dynamic_tag> — the non-templated dynamic type_def ───────────

template <>
class type_def<detail::dynamic_tag> {
    friend class type_instance;

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
        auto to_j = [](const std::any& a) -> nlohmann::json {
            return detail::value_to_json(std::any_cast<const V&>(a));
        };
        auto from_j = [](std::any& a, const nlohmann::json& j) {
            V val{}; detail::value_from_json(j, val); a = std::move(val);
        };
        fields_.push_back({std::string(fname), typeid(V), {}, false, {},
                           std::move(setter), std::move(factory),
                           std::move(to_j), std::move(from_j)});
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
        auto to_j = [](const std::any& a) -> nlohmann::json {
            return detail::value_to_json(std::any_cast<const V&>(a));
        };
        auto from_j = [](std::any& a, const nlohmann::json& j) {
            V val{}; detail::value_from_json(j, val); a = std::move(val);
        };
        detail::dynamic_field_def fd{
            std::string(fname), typeid(V),
            std::any(std::move(default_value)), true, {},
            std::move(setter), std::move(factory),
            std::move(to_j), std::move(from_j)};
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

    field_def field(std::string_view fname) const {
        for (auto& f : fields_)
            if (f.name == fname) return field_def(&f);
        throw std::logic_error(
            "type_def '" + std::string(name_) + "': no field named '" +
            std::string(fname) + "'");
    }

    // ── Field iteration (schema-only) ────────────────────────────────

    template <typename F>
    void for_each_field(F&& fn) const {
        for (auto& f : fields_)
            fn(field_def(&f));
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
        throw std::logic_error(
            "type_def '" + std::string(name_) + "': no meta of requested type");
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

    // ── Meta iteration ─────────────────────────────────────────────

    template <typename F>
    void for_each_meta(F&& fn) const {
        for (auto& e : type_metas_)
            fn(metadata(&e.value, e.type));
    }

    // ── Create instance ─────────────────────────────────────────────

    type_instance create() const;

    // ── Create instance from JSON ────────────────────────────────────
    // Defined in field_json.cpp (module implementation unit).

    type_instance create(const nlohmann::json& j) const;
};

// ── CTAD: type_def("Event") deduces to type_def<detail::dynamic_tag> ─────────────

type_def(const char*) -> type_def<detail::dynamic_tag>;
type_def(std::string_view) -> type_def<detail::dynamic_tag>;

// ═══════════════════════════════════════════════════════════════════════
// type_instance — instance of a dynamic type_def
// ═══════════════════════════════════════════════════════════════════════

class type_instance {
    const type_def<detail::dynamic_tag>* type_;
    std::vector<std::any>        values_;

    int find_field_index(std::string_view name) const {
        auto& fields = type_->fields_;
        for (int i = 0; i < static_cast<int>(fields.size()); ++i)
            if (fields[i].name == name) return i;
        return -1;
    }

public:
    explicit type_instance(const type_def<detail::dynamic_tag>& t) : type_(&t) {
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
    void set(std::string_view name, V&& value) {
        int idx = find_field_index(name);
        if (idx < 0)
            throw std::logic_error(
                "type_instance (type '" + std::string(type_->name_) +
                "'): no field named '" + std::string(name) + "'");
        auto& fd = type_->fields_[idx];
        std::any wrapped(std::forward<V>(value));
        if (fd.setter(values_[idx], std::move(wrapped))) return;
        // Fallback: try string conversion for string-like types
        if constexpr (std::is_constructible_v<std::string, V> &&
                      !std::is_same_v<std::remove_cvref_t<V>, std::string>) {
            std::any str(std::string(std::forward<V>(value)));
            if (fd.setter(values_[idx], std::move(str))) return;
        }
        throw std::logic_error(
            "type_instance (type '" + std::string(type_->name_) +
            "'): field '" + std::string(name) + "' type mismatch");
    }

    // ── Get ──────────────────────────────────────────────────────────

    template <typename V>
    V get(std::string_view name) const {
        int idx = find_field_index(name);
        if (idx < 0)
            throw std::logic_error(
                "type_instance (type '" + std::string(type_->name_) +
                "'): no field named '" + std::string(name) + "'");
        if (auto* p = std::any_cast<V>(&values_[idx]))
            return *p;
        throw std::logic_error(
            "type_instance (type '" + std::string(type_->name_) +
            "'): field '" + std::string(name) + "' type mismatch");
    }

    // ── Has ──────────────────────────────────────────────────────────

    bool has(std::string_view name) const {
        return find_field_index(name) >= 0;
    }

    // ── Type access ──────────────────────────────────────────────────

    const type_def<detail::dynamic_tag>& type() const { return *type_; }

    // ── Field iteration ─────────────────────────────────────────────
    //
    // Callback receives (std::string_view name, const_field_value value)
    // or (std::string_view name, field_value value).
    // Access typed values via value.as<T>() or value.try_as<T>().

    template <typename F>
    void for_each_field(F&& fn) const {
        auto& fields = type_->fields_;
        for (std::size_t i = 0; i < fields.size(); ++i)
            fn(std::string_view(fields[i].name),
               const_field_value(&values_[i]));
    }

    template <typename F>
    void for_each_field(F&& fn) {
        auto& fields = type_->fields_;
        for (std::size_t i = 0; i < fields.size(); ++i)
            fn(std::string_view(fields[i].name),
               field_value(&values_[i]));
    }

    // ── JSON deserialization ─────────────────────────────────────────
    //
    // Overlay semantics: missing keys keep their current/default values.
    // Extra JSON keys are silently ignored. Type mismatches throw.
    // Defined in field_json.cpp (module implementation unit).

    void load_json(const nlohmann::json& j);

    // ── JSON serialization ───────────────────────────────────────────
    //
    // Defined in field_json.cpp (module implementation unit).

    nlohmann::json to_json() const;
    std::string to_json_string(int indent = -1) const;
};

// ── type_def<detail::dynamic_tag>::create() ──────────────────────────────────────

inline type_instance type_def<detail::dynamic_tag>::create() const {
    return type_instance(*this);
}

// ── Typed JSON free functions ────────────────────────────────────────────
//
// to_json / from_json / to_json_string for reflected_struct types.
// These use the same detail::value_to_json / value_from_json that the
// dynamic path's captured lambdas use.

template <detail::reflected_struct T>
nlohmann::json to_json(const T& obj) {
    return detail::value_to_json(obj);
}

template <detail::reflected_struct T>
std::string to_json_string(const T& obj, int indent = -1) {
    auto j = detail::value_to_json(obj);
    return indent < 0 ? j.dump() : j.dump(indent);
}

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

// ── detail::value_to_json / value_from_json implementations ──────────────
//
// Recursive helpers for JSON ↔ C++ value conversion. Handles optionals,
// arrays (vector/set/unordered_set/dense set), maps, reflected_structs,
// enums via magic_enum, and primitives. Used both by the typed free
// functions (to_json/from_json) and by the captured lambdas in dynamic
// field_def.

namespace detail {

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
            for (const auto& [k, val] : v) obj[k] = value_to_json(val);
            return obj;
        } else if constexpr (reflected_struct<T>) {
            nlohmann::json j = nlohmann::json::object();
            type_def<T>{}.for_each_field(v, [&](std::string_view name, const auto& value) {
                j[std::string(name)] = value_to_json(value);
            });
            return j;
        } else if constexpr (std::is_enum_v<T>) {
            std::string_view name = magic_enum::enum_name(v);
            if (name.empty())
                return nlohmann::json(static_cast<std::underlying_type_t<T>>(v));
            return nlohmann::json(std::string(name));
        } else {
            return nlohmann::json(v);
        }
    }

    template <typename T>
    void value_from_json(const nlohmann::json& j, T& out) {
        if constexpr (is_optional_v<T>) {
            using Inner = optional_inner_t<T>;
            if (j.is_null()) { out = std::nullopt; }
            else { Inner inner{}; value_from_json(j, inner); out = std::move(inner); }
        } else if constexpr (is_iterable_array_v<T>) {
            if (!j.is_array()) throw std::logic_error("from_json: expected array");
            using Elem = array_element_t<T>;
            out.clear();
            for (const auto& elem : j) {
                Elem e{}; value_from_json(elem, e);
                if constexpr (requires { out.push_back(e); }) out.push_back(std::move(e));
                else out.insert(std::move(e));
            }
        } else if constexpr (is_map_v<T>) {
            if (!j.is_object()) throw std::logic_error("from_json: expected object for map");
            using V = map_value_t<T>;
            out.clear();
            for (auto& [key, val] : j.items()) {
                V v{}; value_from_json(val, v); out[key] = std::move(v);
            }
        } else if constexpr (reflected_struct<T>) {
            if (!j.is_object()) throw std::logic_error("from_json: expected object for struct");
            type_def<T>{}.for_each_field(out, [&](std::string_view name, auto& value) {
                std::string key(name);
                if (j.contains(key)) value_from_json(j[key], value);
            });
        } else if constexpr (std::is_enum_v<T>) {
            if (j.is_string()) {
                auto str = j.get<std::string>();
                bool found = false;
                for (auto val : magic_enum::enum_values<T>()) {
                    if (magic_enum::enum_name(val) == str) { out = val; found = true; break; }
                }
                if (!found) throw std::logic_error("from_json: unknown enum value '" + str + "'");
            } else if (j.is_number()) {
                out = static_cast<T>(j.get<std::underlying_type_t<T>>());
            } else {
                throw std::logic_error("from_json: expected string or number for enum");
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (!j.is_string()) throw std::logic_error("from_json: expected string");
            out = j.get<std::string>();
        } else if constexpr (std::is_same_v<T, bool>) {
            if (!j.is_boolean()) throw std::logic_error("from_json: expected boolean");
            out = j.get<bool>();
        } else if constexpr (std::is_integral_v<T>) {
            if (!j.is_number()) throw std::logic_error("from_json: expected number");
            out = j.get<T>();
        } else if constexpr (std::is_floating_point_v<T>) {
            if (!j.is_number()) throw std::logic_error("from_json: expected number");
            out = j.get<T>();
        } else {
            out = j.get<T>();
        }
    }

}  // namespace detail

}  // namespace collab::model
