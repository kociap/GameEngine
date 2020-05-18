#ifndef CORE_ATL_DETAIL_TRAITS_COMMON_HPP_INCLUDE
#define CORE_ATL_DETAIL_TRAITS_COMMON_HPP_INCLUDE

#include <core/atl/detail/traits_base.hpp>
#include <core/types.hpp>

namespace anton_engine::atl {
    namespace detail {
        // Is_Array
        //
        template<typename T>
        struct Is_Array: atl::False_Type {};

        template<typename T>
        struct Is_Array<T[]>: atl::True_Type {};

        template<typename T, usize N>
        struct Is_Array<T[N]>: atl::True_Type {};

        // Is_Same
        //
        template<typename T1, typename T2>
        struct Is_Same: atl::False_Type {};

        template<typename T>
        struct Is_Same<T, T>: atl::True_Type {};

        template<typename T1, typename T2>
        constexpr bool is_same = Is_Same<T1, T2>::value;

        // Is_Same_Type
        // Compares 'type' member typedefs.
        // Accesses 'type' member typedef only when the 'value' member is instantiated.
        //
        template<typename T1, typename T2>
        struct Is_Same_Type {
            static constexpr bool value = is_same<typename T1::type, typename T2::type>;
        };
    } // namespace detail
} // namespace anton_engine::atl

#endif // !CORE_ATL_DETAIL_TRAITS_COMMON_HPP_INCLUDE
