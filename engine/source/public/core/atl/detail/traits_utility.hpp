#ifndef CORE_ATL_DETAIL_TRAITS_UTILITY_HPP_INCLUDE
#define CORE_ATL_DETAIL_TRAITS_UTILITY_HPP_INCLUDE

#include <core/atl/detail/traits_transformations.hpp>

namespace anton_engine::atl {
    template<typename T>
    [[nodiscard]] constexpr T&& forward(remove_reference<T>& v) {
        return static_cast<T&&>(v);
    }

    template<typename T>
    [[nodiscard]] constexpr T&& forward(remove_reference<T>&& v) {
        // TODO is there any case where this assert would fail?
        // static_assert(!is_lvalue_reference<T>, "Can not forward an rvalue as an lvalue.");
        return static_cast<T&&>(v);
    }

    template<typename T>
    [[nodiscard]] constexpr remove_reference<T>&& move(T&& v) {
        return static_cast<remove_reference<T>&&>(v);
    }

    template<typename T>
    [[nodiscard]] constexpr add_const<T>& as_const(T& v) {
        return v;
    }

    template<typename T>
    void as_const(T const&&) = delete;
} // namespace anton_engine::atl

#endif // !CORE_ATL_DETAIL_TRAITS_UTILITY_HPP_INCLUDE
