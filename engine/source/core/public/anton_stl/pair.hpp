#ifndef CORE_ANTON_STL_PAIR_HPP_INCLUDE
#define CORE_ANTON_STL_PAIR_HPP_INCLUDE

#include <anton_stl/type_traits.hpp>
#include <anton_stl/detail/utility_common.hpp>
#include <utility>

namespace anton_stl {
    namespace detail {
        template <typename T1, typename U1, typename T2, typename U2>
        inline constexpr bool enable_explicit = is_constructible<T1, U1>&& is_constructible<T2, U2> && !(is_convertible<U1, T1> && is_convertible<U2, T2>);

        template <typename T1, typename U1, typename T2, typename U2>
        inline constexpr bool enable_implicit = is_constructible<T1, U1>&& is_constructible<T2, U2>&& is_convertible<U1, T1>&& is_convertible<U2, T2>;
    } // namespace detail

    template <typename T1, typename T2>
    class Pair {
    public:
        using first_type = T1;
        using second_type = T2;

        first_type first;
        second_type second;

        template <enable_if<is_default_constructible<T1> && is_default_constructible<T2> && detail::is_implicitly_default_constructible<T1> &&
                                detail::is_implicitly_default_constructible<T2>,
                            int> = 0>
        /* [[nodiscard]] */ constexpr Pair(): first(), second() {}
        template <enable_if<is_default_constructible<T1> && is_default_constructible<T2> &&
                                !(detail::is_implicitly_default_constructible<T1> && detail::is_implicitly_default_constructible<T2>),
                            int> = 0>
        /* [[nodiscard]] */ explicit constexpr Pair(): first(), second() {}
        template <typename U1, typename U2, enable_if<detail::enable_implicit<T1, U1&&, T2, U2&&>, int> = 0>
        /* [[nodiscard]] */ constexpr Pair(U1&& a, U2&& b): first(forward<U1>(a)), second(forward<U2>(b)) {}
        template <typename U1, typename U2, enable_if<detail::enable_explicit<T1, U1&&, T2, U2&&>, int> = 0>
        /* [[nodiscard]] */ explicit constexpr Pair(U1&& a, U2&& b): first(forward<U1>(a)), second(forward<U2>(b)) {}
        template <typename U1, typename U2, enable_if<detail::enable_implicit<T1, U1 const&, T2, U2 const&>, int> = 0>
        /* [[nodiscard]] */ constexpr Pair(Pair<U1, U2> const& p): first(p.first), second(p.second) {}
        template <typename U1, typename U2, enable_if<detail::enable_explicit<T1, U1 const&, T2, U2 const&>, int> = 0>
        /* [[nodiscard]] */ explicit constexpr Pair(Pair<U1, U2> const& p): first(p.first), second(p.second) {}
        template <typename U1, typename U2, enable_if<detail::enable_implicit<T1, U1&&, T2, U2&&>, int> = 0>
        /* [[nodiscard]] */ constexpr Pair(Pair<U1, U2>&& p): first(move(p.first)), second(move(p.second)) {}
        template <typename U1, typename U2, enable_if<detail::enable_explicit<T1, U1&&, T2, U2&&>, int> = 0>
        /* [[nodiscard]] */ explicit constexpr Pair(Pair<U1, U2>&& p): first(move(p.first)), second(move(p.second)) {}
        /* [[nodiscard]] */ constexpr Pair(Pair const&) = default;
        /* [[nodiscard]] */ constexpr Pair(Pair&&) = default;
    };

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator==(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return (a.first == b.first) && (a.second == b.second);
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator!=(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return !(a == b);
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return (a.first < b.first) || (!(b.first < a.first) && (a.second < b.second));
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return b < a;
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<=(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return !(a > b);
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>=(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return !(a < b);
    }

    // Tuple_Size
    //
    // Does not support volatile qualified Pair since it has been deprecated in C++20
    //
    template <typename T1, typename T2>
    struct Tuple_Size<Pair<T1, T2>>: Integral_Constant<anton_stl::ssize_t, 2> {};

    template <typename T1, typename T2>
    struct Tuple_Size<Pair<T1, T2> const>: Integral_Constant<anton_stl::ssize_t, 2> {};

    // Tuple_Element
    //
    // Does not support volatile qualifier Pair since it has been deprecates in C++20
    //
    template <anton_stl::ssize_t I, typename T1, typename T2>
    struct Tuple_Element<I, Pair<T1, T2>> {
        static_assert(I == 0 || I == 1, "anton_stl::Pair has only 2 elements");
        using type = conditional<I == 0, T1, T2>;
    };

    template <anton_stl::ssize_t I, typename T1, typename T2>
    struct Tuple_Element<I, Pair<T1, T2> const> {
        static_assert(I == 0 || I == 1, "anton_stl::Pair has only 2 elements");
        using type = conditional<I == 0, T1 const, T2 const>;
    };

    template <typename T1, typename T2>
    inline constexpr enable_if<is_swappable<T1> && is_swappable<T2>> swap(Pair<T1, T2>& a, Pair<T1, T2>& b) {
        swap(a.first, b.first);
        swap(a.second, b.second);
    }

    template <anton_stl::ssize_t N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2>>& get(Pair<T1, T2>& p) {
        static_assert(N == 0 || N == 1, "anton_stl::Pair has only 2 elements");
        if constexpr (N == 0) {
            return p.first;
        } else {
            return p.second;
        }
    }

    template <anton_stl::ssize_t N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2> const>& get(Pair<T1, T2> const& p) {
        static_assert(N == 0 || N == 1, "anton_stl::Pair has only 2 elements");
        if constexpr (N == 0) {
            return p.first;
        } else {
            return p.second;
        }
    }

    template <anton_stl::ssize_t N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2>>&& get(Pair<T1, T2>&& p) {
        static_assert(N == 0 || N == 1, "anton_stl::Pair has only 2 elements");
        if constexpr (N == 0) {
            return move(p.first);
        } else {
            return move(p.second);
        }
    }

    template <anton_stl::ssize_t N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2> const>&& get(Pair<T1, T2> const&& p) {
        static_assert(N == 0 || N == 1, "anton_stl::Pair has only 2 elements");
        if constexpr (N == 0) {
            return move(p.first);
        } else {
            return move(p.second);
        }
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1& get(Pair<T1, T2>& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return p.first;
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1& get(Pair<T2, T1>& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return p.second;
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const& get(Pair<T1, T2> const& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return p.first;
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const& get(Pair<T2, T1> const& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return p.second;
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1&& get(Pair<T1, T2>&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return move(p.first);
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1&& get(Pair<T2, T1>&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return move(p.second);
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const&& get(Pair<T1, T2> const&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return move(p.first);
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const&& get(Pair<T2, T1> const&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with anton_stl::Pair with the same types.");
        return move(p.second);
    }
} // namespace anton_stl

// We provide std::tuple_size and std::tuple_element to enable structured bindings
namespace std {
    template <typename T1, typename T2>
    struct tuple_size<anton_stl::Pair<T1, T2>>: anton_stl::Tuple_Size<anton_stl::Pair<T1, T2>> {};

    template <typename T1, typename T2>
    struct tuple_size<anton_stl::Pair<T1, T2> const>: anton_stl::Tuple_Size<anton_stl::Pair<T1, T2> const> {};

    template <anton_stl::ssize_t I, typename T1, typename T2>
    struct tuple_element<I, anton_stl::Pair<T1, T2>>: anton_stl::Tuple_Element<I, anton_stl::Pair<T1, T2>> {};

    template <anton_stl::ssize_t I, typename T1, typename T2>
    struct tuple_element<I, anton_stl::Pair<T1, T2> const>: anton_stl::Tuple_Element<I, anton_stl::Pair<T1, T2> const> {};
} // namespace std

#endif //! CORE_ANTON_STL_PAIR_HPP_INCLUDE
