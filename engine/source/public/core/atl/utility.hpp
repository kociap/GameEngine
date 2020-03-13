#ifndef CORE_ATL_UTILITY_HPP_INCLUDE
#define CORE_ATL_UTILITY_HPP_INCLUDE

#include <core/atl/detail/utility_common.hpp>

// TODO: Is this header necessary?

namespace anton_engine::atl {
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
} // namespace anton_engine::atl

#endif // !CORE_ATL_UTILITY_HPP_INCLUDE
