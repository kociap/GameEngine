#ifndef CORE_ATL_TUPLE_HPP_INCLUDE
#define CORE_ATL_TUPLE_HPP_INCLUDE

#include <core/atl/detail/utility_common.hpp>
#include <core/atl/tags.hpp>
#include <core/types.hpp>

namespace anton_engine::atl {
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

    template<typename T, T N>
    struct Make_Integer_Sequence {
        using type = typename detail::Make_Integer_Sequence<T, N, 0, Integer_Sequence<T>>::type;
    };

    template<typename T, T N>
    using make_integer_sequence = typename Make_Integer_Sequence<T, N>::type;

    namespace detail {
        template<u64 Index, typename Type>
        class Tuple_Child {
        public:
            template<typename T>
            constexpr Tuple_Child(T&& arg): _element(atl::forward<T>(arg)) {}

            Type _element;
        };

        template<typename, typename...>
        class Tuple_Expand;

        template<u64... Indices, typename... Types>
        class Tuple_Expand<Integer_Sequence<u64, Indices...>, Types...>: public Tuple_Child<Indices, Types>... {
        public:
            template<typename... Args>
            constexpr Tuple_Expand(Variadic_Construct_Tag, Args&&... args): Tuple_Child<Indices, Types>(atl::forward<Args>(args))... {}
        };
    } // namespace detail

    template<typename... Ts>
    class Tuple: public detail::Tuple_Expand<make_integer_sequence<u64, sizeof...(Ts)>, Ts...> {
    private:
        using base_t = detail::Tuple_Expand<make_integer_sequence<u64, sizeof...(Ts)>, Ts...>;

    public:
        // TODO: Add conditional explicit
        template<typename... Args, enable_if<sizeof...(Args) == sizeof...(Ts) && (sizeof...(Args) > 0), int> = 0>
        constexpr Tuple(Args&&... args): base_t(variadic_construct, atl::forward<Args>(args)...) {}
    };

    template<typename... Types>
    struct Tuple_Size<Tuple<Types...>>: Integral_Constant<u64, sizeof...(Types)> {};

    template<typename... Types>
    struct Tuple_Size<Tuple<Types...> const>: Integral_Constant<u64, sizeof...(Types)> {};

    namespace detail {
        template<typename T, typename... Ts>
        struct Tuple_Type_Slitter {
            using first = T;
            using rest = Tuple<Ts...>;
        };

        template<u64 N, u64 X, typename>
        struct Tuple_Element;

        template<u64 N, u64 X, typename T, typename... Types>
        struct Tuple_Element<N, X, Tuple<T, Types...>> {
            using type = typename Tuple_Element<N, X + 1, Tuple<Types...>>::type;
        };

        template<u64 N, typename T, typename... Types>
        struct Tuple_Element<N, N, Tuple<T, Types...>> {
            using type = T;
        };
    } // namespace detail

    template<u64 Index, typename... Types>
    struct Tuple_Element<Index, Tuple<Types...>> {
        using type = typename detail::Tuple_Element<Index, 0, Tuple<Types...>>::type;
    };

    template<u64 Index, typename... Types>
    struct Tuple_Element<Index, Tuple<Types...> const> {
        using type = atl::add_const<typename detail::Tuple_Element<Index, 0, Tuple<Types...>>::type>;
    };

    namespace detail {
        template<typename T, u64 Index>
        constexpr T& get_helper_type(Tuple_Child<Index, T>& element) {
            return static_cast<T&>(element._element);
        }

        template<typename T, u64 Index>
        constexpr T const& get_helper_type(Tuple_Child<Index, T> const& element) {
            return static_cast<T const&>(element._element);
        }

        template<typename T, u64 Index>
        constexpr T&& get_helper_type(Tuple_Child<Index, T>&& element) {
            return static_cast<T&&>(element._element);
        }

        template<typename T, u64 Index>
        constexpr T const&& get_helper_type(Tuple_Child<Index, T> const&& element) {
            return static_cast<T const&&>(element._element);
        }
    } // namespace detail

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>>& get(Tuple<Types...>& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type&>(static_cast<detail::Tuple_Child<Index, type>&>(t)._element);
    }

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>> const& get(Tuple<Types...> const& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type const&>(static_cast<detail::Tuple_Child<Index, type> const&>(t)._element);
    }

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>>&& get(Tuple<Types...>&& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type&&>(static_cast<detail::Tuple_Child<Index, type>&&>(t)._element);
    }

    template<u64 Index, typename... Types>
    constexpr tuple_element<Index, Tuple<Types...>> const&& get(Tuple<Types...> const&& t) {
        using type = tuple_element<Index, Tuple<Types...>>;
        return static_cast<type const&&>(static_cast<detail::Tuple_Child<Index, type> const&&>(t)._element);
    }

    template<typename T, typename... Types>
    constexpr T& get(Tuple<Types...>& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename T, typename... Types>
    constexpr T const& get(Tuple<Types...> const& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename T, typename... Types>
    constexpr T&& get(Tuple<Types...>&& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename T, typename... Types>
    constexpr T const&& get(Tuple<Types...> const&& t) {
        return detail::get_helper_type<T>(t);
    }

    template<typename... Types>
    constexpr Tuple<atl::decay<Types>...> make_tuple(Types&&... args) {
        return Tuple<atl::decay<Types>...>(atl::forward<Types>(args)...);
    }

    namespace detail {
        template<typename Callable, typename Tuple, u64... Indices>
        constexpr decltype(auto) apply(Callable&& callable, Tuple&& tuple, integer_sequence<u64, Indices...>) {
            return callable(atl::get<Indices>(atl::forward<Tuple>(tuple))...);
        }
    } // namespace detail

    template<typename Callable, typename Tuple>
    constexpr decltype(auto) apply(Callable&& callable, Tuple&& tuple) {
        return detail::apply(atl::forward<Callable>(callable), atl::forward<Tuple>(tuple),
                             make_integer_sequence<u64, atl::tuple_size<atl::remove_reference<Tuple>>>());
    }
} // namespace anton_engine::atl

// We provide std::tuple_size and std::tuple_element to enable structured bindings
namespace std {
    template<typename... Types>
    struct tuple_size<anton_engine::atl::Tuple<Types...>>: anton_engine::atl::Tuple_Size<anton_engine::atl::Tuple<Types...>> {};

    template<typename... Types>
    struct tuple_size<anton_engine::atl::Tuple<Types...> const>: anton_engine::atl::Tuple_Size<anton_engine::atl::Tuple<Types...> const> {};

    template<unsigned long long I, typename... Types>
    struct tuple_element<I, anton_engine::atl::Tuple<Types...>>: anton_engine::atl::Tuple_Element<I, anton_engine::atl::Tuple<Types...>> {};

    template<unsigned long long I, typename... Types>
    struct tuple_element<I, anton_engine::atl::Tuple<Types...> const>: anton_engine::atl::Tuple_Element<I, anton_engine::atl::Tuple<Types...> const> {};
} // namespace std

#endif // !CORE_ATL_TUPLE_HPP_INCLUDE
