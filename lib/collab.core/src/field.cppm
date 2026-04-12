module;

#include <boost/pfr.hpp>

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

export module collab.core:field;

import :fixed_string;
import :options;

// ═══════════════════════════════════════════════════════════════════════════
// ⚠️  MSVC 14.50 / VS 2026 COMDAT bug — known gotcha for users of Field<>
// ═══════════════════════════════════════════════════════════════════════════
//
// Symptom: LNK1179: invalid or corrupt file: duplicate COMDAT '??0…Field…'
//          (or '??1…Field…' for the destructor).
//
// Trigger: a single translation unit textually references the *same*
//          Field<Name, T, Core, Exts...> specialization more than once,
//          AND `T` is non-trivially-destructible (e.g. std::string,
//          std::vector, std::filesystem::path, any user class with a
//          non-trivial dtor).
//
// Example that breaks:
//
//     // all in one .cpp / .cppm:
//     struct LoginResponse  { Field<"session_id", std::string> session_id; };
//     struct LogoutResponse { Field<"session_id", std::string> session_id; };
//
// Both structs reference the same Field specialization in one TU, and MSVC
// 14.50 emits an implicit dtor COMDAT section for each textual reference
// instead of reusing one. The linker sees two sections with identical
// COMDAT names in the same .obj and bails with LNK1179.
//
// Not an issue on:
//   - GCC / Clang (any version)
//   - MSVC < 14.50
//   - Field<Name, T, ...> with trivially-destructible T (int, bool, enum…)
//   - Different Field specializations in the same TU (distinct Names)
//   - The same specialization spread across multiple TUs (normal COMDAT
//     folding handles that)
//
// Workarounds (pick whichever fits your codebase):
//   1. Put each struct that owns the shared Field specialization in its
//      own translation unit. This is the natural xmake/modules layout
//      and costs nothing.
//   2. Make the Name NTTPs unique: `Field<"login_session_id", std::string>`
//      vs `Field<"logout_session_id", std::string>`. Avoid if possible —
//      the whole point of Field<> is that Name is a real-world-meaningful
//      identifier, not a compile-dodge.
//
// Field<> itself is correct. We have confirmed the bug is purely a MSVC
// codegen defect — exhaustively tested with:
//   • clean rebuild (does not fix)
//   • explicit `= default` of all special members (does not fix)
//   • user-provided special member bodies (does not fix)
//   • base class storage parameterized only by T (does not fix)
// The fix must come from Microsoft. A minimal repro has been prepared.
// ═══════════════════════════════════════════════════════════════════════════

export namespace collab::core {

namespace detail {

    // Count how many times T appears in Ts... at compile time.
    template <typename T, typename... Ts>
    consteval std::size_t count_type() {
        return (std::size_t{0} + ... + (std::is_same_v<T, Ts> ? std::size_t{1} : std::size_t{0}));
    }

    // True if every type in Ts... appears exactly once.
    template <typename... Ts>
    consteval bool all_unique() {
        return ((count_type<Ts, Ts...>() == std::size_t{1}) && ...);
    }

}  // namespace detail

// Reflection primitive: wraps a struct member with compile-time metadata.
//
//   Name  — compile-time field name (fixed_string NTTP)
//   T     — wrapped value type
//   Core  — generic metadata bag (desc, required, display_name, hidden, …)
//   Exts… — variadic pack of arbitrary typed extension bags attached at
//           declaration time; each must be of a structural type, and each
//           type may appear at most once in the pack.
//
// Field is deliberately an aggregate so outer structs containing Field
// members remain aggregate-initializable with designated initializers.
template <fixed_string Name, typename T, options Core = {}, auto... Exts>
struct Field {
    static_assert(
        detail::all_unique<std::decay_t<decltype(Exts)>...>(),
        "Field<> declared with duplicate extension types. "
        "Combine them into a single instance — e.g., one options-style bag "
        "per subsystem, not two."
    );

    // Sentinel used by IsField. Variadic `auto...` NTTP packs don't deduce
    // cleanly through trait partial specializations on MSVC, so we detect
    // Field<> by the presence of this marker instead.
    static constexpr bool _collab_core_field_marker = true;

    T value{};

    static consteval auto    field_name() noexcept { return Name; }
    using                    type = T;
    static consteval options field_core() noexcept { return Core; }

    // Transparent conversions — behaves like the wrapped T.
    constexpr operator const T&() const& noexcept { return value; }
    constexpr operator       T&()       & noexcept { return value; }
    constexpr operator       T&&()      && noexcept { return std::move(value); }

    // Transparent assignment from a T — non-template overloads rather than
    // a forwarding template, because MSVC 14.50's linker trips on duplicate
    // COMDATs for template member functions defined in a module interface
    // unit (LNK1179). Implicit conversions into T still kick in at the call
    // site, so `field = "hello"` for Field<…, std::string> still works.
    constexpr Field& operator=(const T& v) {
        value = v;
        return *this;
    }
    constexpr Field& operator=(T&& v) noexcept(std::is_nothrow_move_assignable_v<T>) {
        value = std::move(v);
        return *this;
    }

    // Method-call passthrough so `f->size()` works when T supports it.
    constexpr const T* operator->() const noexcept { return &value; }
    constexpr       T* operator->()       noexcept { return &value; }

    // Compile-time extension lookup. Returns the extension of type E, or a
    // default-constructed E if none. Duplicate extension types are rejected
    // at declaration time, so at most one match exists per lookup.
    template <typename E>
    static consteval E ext() {
        E result{};
        ([&] {
            if constexpr (std::is_same_v<std::decay_t<decltype(Exts)>, E>) {
                result = Exts;
            }
        }(), ...);
        return result;
    }

    // True if any extension in the pack has type E.
    template <typename E>
    static consteval bool has_ext() {
        return ((std::is_same_v<std::decay_t<decltype(Exts)>, E>) || ...);
    }
};

// ── Concept: is T a specialization of Field<>? ─────────────────────────

namespace detail {

    template <typename T>
    concept has_field_marker = requires { T::_collab_core_field_marker; };

}  // namespace detail

template <typename T>
concept IsField = detail::has_field_marker<std::remove_cvref_t<T>>;

// ── Concept: ReflectedStruct ─────────────────────────────────────────────
//
// A reflected struct is an aggregate with at least one Field<> member.
// Non-Field members are allowed and silently ignored by the framework.
// Walks members via boost::pfr at compile time.

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

}  // namespace collab::core
