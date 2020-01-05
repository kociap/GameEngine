#ifndef CORE_ANTON_STL_UTILITY_COMMON_HPP_INCLUDE
#define CORE_ANTON_STL_UTILITY_COMMON_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/type_traits.hpp>

namespace anton_engine::anton_stl {
    template <typename T>
    inline constexpr enable_if<is_move_constructible<T> && is_move_assignable<T>, void> swap(T& a, T& b) {
        T tmp(anton_stl::move(a));
        a = anton_stl::move(b);
        b = anton_stl::move(tmp);
    }

    template <typename T, typename U, typename = void>
    struct Is_Swappable_With: False_Type {};

    // Note: swap left unprefixed to allow ADL of user-defined swap specializations
    template <typename T, typename U>
    struct Is_Swappable_With<T, U, void_trait<decltype(swap(declval<U&>(), declval<T&>())), decltype(swap(declval<T&>(), declval<U&>()))>>: True_Type {};

    template <typename T, typename U>
    inline constexpr bool is_swappable_with = Is_Swappable_With<T, U>::value;

    template <typename T>
    struct Is_Swappable: Is_Swappable_With<T, T> {};

    template <typename T>
    inline constexpr bool is_swappable = Is_Swappable<T>::value;

    template <typename>
    struct Tuple_Size;

    template <usize, typename>
    struct Tuple_Element;

    template <usize I, typename T>
    using tuple_element = typename Tuple_Element<I, T>::type;
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_UTILITY_COMMON_HPP_INCLUDE
