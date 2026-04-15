module;

#ifdef COLLAB_FIELD_HAS_PFR
#include <pfr.hpp>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>
#endif

module collab.core:field_pfr;

import :field;

#ifdef COLLAB_FIELD_HAS_PFR

namespace collab::field::pfr_impl {

template <typename T>
consteval std::size_t field_count() {
    return pfr::tuple_size_v<T>;
}

template <std::size_t I, typename T>
consteval std::string_view field_name() {
    return pfr::get_name<I, T>();
}

template <std::size_t I, typename T>
using member_type = pfr::tuple_element_t<I, T>;

template <std::size_t I, typename T>
constexpr decltype(auto) get_member(T& obj) {
    return pfr::get<I>(obj);
}

template <typename T, typename F>
constexpr void for_each_member(T& obj, F&& fn) {
    pfr::for_each_field(obj, std::forward<F>(fn));
}

}  // namespace collab::field::pfr_impl

#endif
