#ifndef CORE_ANTON_STL_DETAIL_TRAITS_BASE_HPP_INCLUDE
#define CORE_ANTON_STL_DETAIL_TRAITS_BASE_HPP_INCLUDE

namespace anton_engine::anton_stl {
    // Integral_Constant
    //
    template <typename T, T V>
    struct Integral_Constant {
        using type = Integral_Constant<T, V>;
        using value_type = T;

        constexpr static T value = V;

        constexpr operator value_type() const {
            return V;
        }

        constexpr value_type operator()() const {
            return V;
        }
    };

    template <bool V>
    using Bool_Constant = Integral_Constant<bool, V>;

    using True_Type = Bool_Constant<true>;
    using False_Type = Bool_Constant<false>;

    // Enable_If
    //
    template <bool B, typename = void>
    struct Enable_If {};

    template <typename T>
    struct Enable_If<true, T> {
        using type = T;
    };

    template <bool B, typename T = void>
    using enable_if = typename Enable_If<B, T>::type;

    // Disable_If
    //
    template <bool B, typename = void>
    struct Disable_If {};

    template <typename T>
    struct Disable_If<false, T> {
        using type = T;
    };

    template <bool B, typename T = void>
    using disable_if = typename Disable_If<B, T>::type;

    // Conditional
    //
    template <bool, typename T1, typename T2>
    struct Conditional {
        using type = T2;
    };

    template <typename T1, typename T2>
    struct Conditional<true, T1, T2> {
        using type = T1;
    };

    template <bool B, typename T1, typename T2>
    using conditional = typename Conditional<B, T1, T2>::type;

    // void_trait
    // Maps a collection of types to void.
    //
    template <typename...>
    using void_trait = void;

    // type_sink
    // Maps a collection of types to a user-provided type T.
    //
    template <typename T, typename...>
    using type_sink = T;

    // true_trait
    //
    template <typename...>
    constexpr bool true_trait = true;

    // false_trait
    //
    template <typename...>
    constexpr bool false_trait = false;
    //
    // Logical operators
    //

    // Negation
    // Unary trait that performs logical NOT on a single type trait.
    //
    // https://en.cppreference.com/w/cpp/types/negation
    //
    template <typename T>
    struct Negation: Bool_Constant<!bool(T::value)> {};

    template <typename T>
    constexpr bool negation = Negation<T>::value;

    // Conjunction
    // Trait that performs a short-circuiting logical AND on a sequence of type traits.
    // Inherits from the first trait that whose `value` is false or the last trait otherwise.
    // If the parameter pack is empty, inherits from anton_stl::True_Type.
    // Requires the types to be useable as a base class and expects them to provide `value` member.
    //
    // https://en.cppreference.com/w/cpp/types/conjunction
    //
    template <typename... T>
    struct Conjunction: True_Type {};

    template <typename T, typename... Ts>
    struct Conjunction<T, Ts...>: conditional<!bool(T::value), T, Conjunction<Ts...>> {};

    template <typename T>
    struct Conjunction<T>: T {};

    template <typename... T>
    constexpr bool conjunction = Conjunction<T...>::value;

    // Disjunction
    // Trait that performs a short-circuiting logical OR on a sequence of type traits.
    // Inherits from the first trait that whose `value` is true or the last trait otherwise.
    // If the parameter pack is empty, inherits from anton_stl::False_Type.
    // Requires the types to be useable as a base class and expects them to provide `value` member.
    //
    // https://en.cppreference.com/w/cpp/types/disjunction
    //
    template <typename... T>
    struct Disjunction: False_Type {};

    template <typename T, typename... Ts>
    struct Disjunction<T, Ts...>: conditional<bool(T::value), T, Disjunction<Ts...>> {};

    template <typename T>
    struct Disjunction<T>: T {};

    template <typename... T>
    constexpr bool disjunction = Disjunction<T...>::value;

    // declval
    // Intentionally left unimplemented because it is intended to be used only in unevaluated context.
    template <typename T>
    T&& declval();
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_DETAIL_TRAITS_BASE_HPP_INCLUDE
