#ifndef CORE_ATL_DETAIL_FUNCTORS_HPP_INCLUDE
#define CORE_ATL_DETAIL_FUNCTORS_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/type_traits.hpp>
#include <core/hashing/murmurhash2.hpp>

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
    }

    // Default_Hash
    // TODO: long double, nullptr, separate enum, int128 specializations
    //
    template<typename T>
    struct Default_Hash: detail::Conditionally_Enabled_Hash<!is_const<T> && (is_enum<T> || is_integral<T> || is_floating_point<T> || is_pointer<T>)> {
        [[nodiscard]] u64 operator()(T const& v) {
            union {
                T t;
                u64 v;
            } u;
            u.t = v;
            return murmurhash2_64(&u.v, sizeof(u64), 0x1F0D3804);
        }
    };

    // Equal_Compare
    //
    template<typename T>
    struct Equal_Compare {
        [[nodiscard]] bool operator()(T const& lhs, T const& rhs) {
            return lhs == rhs;
        }
    };
} // namespace anton_engine

#endif // !CORE_ATL_DETAIL_FUNCTORS_HPP_INCLUDE
