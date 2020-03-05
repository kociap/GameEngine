#ifndef CORE_ANTON_STL_DETAIL_TRAITS_COMMON_HPP_INCLUDE
#define CORE_ANTON_STL_DETAIL_TRAITS_COMMON_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/detail/traits_base.hpp>

namespace anton_engine::anton_stl {
    namespace detail {
        // Is_Array
        //
        template <typename T>
        struct Is_Array: anton_stl::False_Type {};

        template <typename T>
        struct Is_Array<T[]>: anton_stl::True_Type {};

        template <typename T, usize N>
        struct Is_Array<T[N]>: anton_stl::True_Type {};

        // Is_Same
        //
        template <typename T1, typename T2>
        struct Is_Same: anton_stl::False_Type {};

        template <typename T>
        struct Is_Same<T, T>: anton_stl::True_Type {};

        template <typename T1, typename T2>
        constexpr bool is_same = Is_Same<T1, T2>::value;
    } // namespace detail
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_DETAIL_TRAITS_COMMON_HPP_INCLUDE
