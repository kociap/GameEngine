#ifndef CORE_ATL_PAIR_HPP_INCLUDE
#define CORE_ATL_PAIR_HPP_INCLUDE

#include <core/atl/detail/utility_common.hpp>
#include <core/atl/type_traits.hpp>
#include <core/types.hpp>

namespace anton_engine::atl {
    template<typename T1, typename T2>
    class Pair {
    public:
        using first_type = T1;
        using second_type = T2;

        first_type first;
        second_type second;

        constexpr Pair() = default;
        template<typename U1, typename U2>
        constexpr Pair(U1&& u1, U2&& u2): first(atl::forward<U1>(u1)), second(atl::forward<U2>(u2)) {}
        template<typename U1, typename U2>
        constexpr Pair(Pair<U1, U2> const& pair): first(pair.first), second(pair.second) {}
        template<typename U1, typename U2>
        constexpr Pair(Pair<U1, U2>&& pair): first(atl::move(pair.first)), second(atl::move(pair.second)) {}
        constexpr Pair(Pair const& pair) = default;
        constexpr Pair(Pair&& pair) = default;
        constexpr Pair& operator=(Pair const& pair) = default;
        constexpr Pair& operator=(Pair&& pair) = default;
        constexpr ~Pair() = default;
    };

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator==(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return (a.first == b.first) && (a.second == b.second);
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator!=(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return !(a == b);
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return (a.first < b.first) || (!(b.first < a.first) && (a.second < b.second));
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return b < a;
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<=(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return !(a > b);
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>=(Pair<T1, T2> const& a, Pair<T1, T2> const& b) {
        return !(a < b);
    }

    // Tuple_Size
    //
    // Does not support volatile qualified Pair since it has been deprecated in C++20
    //
    template<typename T1, typename T2>
    struct Tuple_Size<Pair<T1, T2>>: Integral_Constant<usize, 2> {};

    template<typename T1, typename T2>
    struct Tuple_Size<Pair<T1, T2> const>: Integral_Constant<usize, 2> {};

    // Tuple_Element
    //
    // Does not support volatile qualifier Pair since it has been deprecates in C++20
    //
    template<usize I, typename T1, typename T2>
    struct Tuple_Element<I, Pair<T1, T2>> {
        static_assert(I == 0 || I == 1, "atl::Pair has only 2 elements");
        using type = conditional<I == 0, T1, T2>;
    };

    template<usize I, typename T1, typename T2>
    struct Tuple_Element<I, Pair<T1, T2> const> {
        static_assert(I == 0 || I == 1, "atl::Pair has only 2 elements");
        using type = conditional<I == 0, T1 const, T2 const>;
    };

    template<typename T1, typename T2>
    inline constexpr enable_if<is_swappable<T1> && is_swappable<T2>> swap(Pair<T1, T2>& a, Pair<T1, T2>& b) {
        swap(a.first, b.first);
        swap(a.second, b.second);
    }

    template<usize N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2>>& get(Pair<T1, T2>& p) {
        static_assert(N == 0 || N == 1, "atl::Pair has only 2 elements");
        if constexpr(N == 0) {
            return p.first;
        } else {
            return p.second;
        }
    }

    template<usize N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2> const>& get(Pair<T1, T2> const& p) {
        static_assert(N == 0 || N == 1, "atl::Pair has only 2 elements");
        if constexpr(N == 0) {
            return p.first;
        } else {
            return p.second;
        }
    }

    template<usize N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2>>&& get(Pair<T1, T2>&& p) {
        static_assert(N == 0 || N == 1, "atl::Pair has only 2 elements");
        if constexpr(N == 0) {
            return move(p.first);
        } else {
            return move(p.second);
        }
    }

    template<usize N, typename T1, typename T2>
    [[nodiscard]] inline constexpr tuple_element<N, Pair<T1, T2> const>&& get(Pair<T1, T2> const&& p) {
        static_assert(N == 0 || N == 1, "atl::Pair has only 2 elements");
        if constexpr(N == 0) {
            return move(p.first);
        } else {
            return move(p.second);
        }
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1& get(Pair<T1, T2>& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return p.first;
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1& get(Pair<T2, T1>& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return p.second;
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const& get(Pair<T1, T2> const& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return p.first;
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const& get(Pair<T2, T1> const& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return p.second;
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1&& get(Pair<T1, T2>&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return move(p.first);
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1&& get(Pair<T2, T1>&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return move(p.second);
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const&& get(Pair<T1, T2> const&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return move(p.first);
    }

    template<typename T1, typename T2>
    [[nodiscard]] inline constexpr T1 const&& get(Pair<T2, T1> const&& p) {
        static_assert(!is_same<T1, T2>, "Type based get may not be called with atl::Pair with the same types.");
        return move(p.second);
    }
} // namespace anton_engine::atl

// We provide std::tuple_size and std::tuple_element to enable structured bindings
namespace std {
    template<typename T1, typename T2>
    struct tuple_size<anton_engine::atl::Pair<T1, T2>>: anton_engine::atl::Tuple_Size<anton_engine::atl::Pair<T1, T2>> {};

    template<typename T1, typename T2>
    struct tuple_size<anton_engine::atl::Pair<T1, T2> const>: anton_engine::atl::Tuple_Size<anton_engine::atl::Pair<T1, T2> const> {};

    template<anton_engine::usize I, typename T1, typename T2>
    struct tuple_element<I, anton_engine::atl::Pair<T1, T2>>: anton_engine::atl::Tuple_Element<I, anton_engine::atl::Pair<T1, T2>> {};

    template<anton_engine::usize I, typename T1, typename T2>
    struct tuple_element<I, anton_engine::atl::Pair<T1, T2> const>: anton_engine::atl::Tuple_Element<I, anton_engine::atl::Pair<T1, T2> const> {};
} // namespace std

#endif //! CORE_ATL_PAIR_HPP_INCLUDE
