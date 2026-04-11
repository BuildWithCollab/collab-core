module;

#include <algorithm>
#include <cstddef>

export module collab.core:fixed_string;

export namespace collab::core {

// N comes from a string literal and includes the null terminator, so size() returns N - 1.
template <std::size_t N>
struct fixed_string {
    char value[N];

    constexpr fixed_string(const char (&s)[N]) { std::copy_n(s, N, value); }

    constexpr std::size_t size() const noexcept { return N - 1; }
    constexpr const char* c_str() const noexcept { return value; }

    constexpr bool operator==(const fixed_string&) const = default;
};

template <std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

}  // namespace collab::core
