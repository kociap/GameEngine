#ifndef CORE_ATL_DETAIL_FUNCTORS_HPP_INCLUDE
#define CORE_ATL_DETAIL_FUNCTORS_HPP_INCLUDE

#include <core/atl/type_traits.hpp>
#include <core/hashing/murmurhash2.hpp>
#include <core/types.hpp>

namespace anton_engine::atl {
    namespace detail {
        template<bool enabled>
        struct Conditionally_Enabled_Hash {};

        template<>
        struct Conditionally_Enabled_Hash<false> {
            Conditionally_Enabled_Hash() = delete;
            Conditionally_Enabled_Hash(Conditionally_Enabled_Hash const&) = delete;
            Conditionally_Enabled_Hash(Conditionally_Enabled_Hash&&) = delete;
            ~Conditionally_Enabled_Hash() = delete;
            Conditionally_Enabled_Hash& operator=(Conditionally_Enabled_Hash const&) = delete;
            Conditionally_Enabled_Hash& operator=(Conditionally_Enabled_Hash&&) = delete;
        };
    } // namespace detail

    // Default_Hash
    // TODO: long double, nullptr, int128 specializations
    //
    template<typename T>
    struct Default_Hash: detail::Conditionally_Enabled_Hash<!is_const<T> && (is_enum<T> || is_integral<T> || is_floating_point<T> || is_pointer<T>)> {
        [[nodiscard]] constexpr u64 operator()(T const& v) const {
            return murmurhash2_64(&v, sizeof(T));
        }
    };

    // Equal_Compare
    //
    template<typename T>
    struct Equal_Compare {
        [[nodiscard]] constexpr bool operator()(T const& lhs, T const& rhs) const {
            return lhs == rhs;
        }
    };

    // Default_Deleter
    //
    template<typename T>
    struct Default_Deleter {
        void operator()(T* pointer) {
            delete pointer;
        }
    };

    // Is_Transparent
    // Determines whether a type has a member typedef `transparent`.
    // Used by containers to determine whether hash and compare functors
    // may be used for heterogenous lookup.
    //
    template<typename T, typename = void>
    struct Is_Transparent: public atl::False_Type {};

    template<typename T>
    struct Is_Transparent<T, atl::void_sink<typename T::transparent>>: public atl::True_Type {};

    template<typename T>
    constexpr bool is_transparent = Is_Transparent<T>::value;
} // namespace anton_engine::atl

#endif // !CORE_ATL_DETAIL_FUNCTORS_HPP_INCLUDE
