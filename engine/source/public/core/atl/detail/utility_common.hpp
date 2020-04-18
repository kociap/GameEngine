#ifndef CORE_ATL_UTILITY_COMMON_HPP_INCLUDE
#define CORE_ATL_UTILITY_COMMON_HPP_INCLUDE

#include <core/atl/type_traits.hpp>
#include <core/types.hpp>

namespace anton_engine::atl {
    template<typename T>
    inline constexpr void swap(T& a, T& b) {
        T tmp(atl::move(a));
        a = atl::move(b);
        b = atl::move(tmp);
    }

    template<typename T, typename U, typename = void>
    struct Is_Swappable_With: False_Type {};

    // Note: swap left unprefixed to allow ADL of user-defined swap specializations
    template<typename T, typename U>
    struct Is_Swappable_With<T, U, void_trait<decltype(swap(declval<U&>(), declval<T&>())), decltype(swap(declval<T&>(), declval<U&>()))>>: True_Type {};

    template<typename T, typename U>
    inline constexpr bool is_swappable_with = Is_Swappable_With<T, U>::value;

    template<typename T>
    struct Is_Swappable: Is_Swappable_With<T, T> {};

    template<typename T>
    inline constexpr bool is_swappable = Is_Swappable<T>::value;

    template<typename>
    struct Tuple_Size;

    template<typename T>
    constexpr u64 tuple_size = Tuple_Size<T>::value;

    template<usize, typename>
    struct Tuple_Element;

    template<usize I, typename T>
    using tuple_element = typename Tuple_Element<I, T>::type;

    // Integer_Sequence
    //
    template<typename T, T... Integers>
    struct Integer_Sequence {
        using type = Integer_Sequence<T, Integers...>;
    };

    template<typename T, T... Integers>
    using integer_sequence = Integer_Sequence<T, Integers...>;

    namespace detail {
        template<typename T, T, T, typename>
        struct Make_Integer_Sequence;

        template<typename T, T N, T X, T... Integers>
        struct Make_Integer_Sequence<T, N, X, Integer_Sequence<T, Integers...>> {
            using type = typename Make_Integer_Sequence<T, N, X + 1, Integer_Sequence<T, Integers..., X>>::type;
        };

        template<typename T, T N, T... Integers>
        struct Make_Integer_Sequence<T, N, N, Integer_Sequence<T, Integers...>> {
            using type = Integer_Sequence<T, Integers...>;
        };
    } // namespace detail

    // Make_Integer_Sequence
    // Creates an Integer_Sequence which starts at 0 and ends at N - 1.
    //
    template<typename T, T N>
    struct Make_Integer_Sequence {
        using type = typename detail::Make_Integer_Sequence<T, N, 0, Integer_Sequence<T>>::type;
    };

    template<typename T, T N>
    using make_integer_sequence = typename Make_Integer_Sequence<T, N>::type;
} // namespace anton_engine::atl

// We provide std::tuple_size and std::tuple_element to enable structured bindings on certain types
// so we forward declare those types so that we do not have to deal with errors or std headers.
namespace std {
    template<typename>
    struct tuple_size;

    template<anton_engine::usize, typename>
    struct tuple_element;
} // namespace std

#endif // !CORE_ATL_UTILITY_COMMON_HPP_INCLUDE
