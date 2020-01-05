#ifndef CORE_ANTON_STL_UTILITY_HPP_INCLUDE
#define CORE_ANTON_STL_UTILITY_HPP_INCLUDE

#include <anton_stl/detail/utility_common.hpp>

// TODO: Is this header necessary?

namespace anton_engine::anton_stl {
    template <typename C>
    constexpr auto size(C& c) -> decltype(c.size()) {
        return c.size();
    }

    template <typename T, isize N>
    constexpr isize size(T (&)[N]) {
        return N;
    }

    template <typename C>
    constexpr auto data(C& c) -> decltype(c.data()) {
        return c.data();
    }

    template <typename T, usize N>
    constexpr T* data(T (&array)[N]) {
        return array;
    }
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_UTILITY_HPP_INCLUDE
