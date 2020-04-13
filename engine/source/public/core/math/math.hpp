#ifndef CORE_MATH_MATH_HPP_INCLUDE
#define CORE_MATH_MATH_HPP_INCLUDE

#include <core/anton_crt.hpp>
#include <core/types.hpp>

// Safety measures
#undef max
#undef min
#undef far
#undef near

namespace anton_engine::math {
    constexpr f32 pi = 3.1415926f;
    constexpr f32 half_pi = 1.5707964f;
    constexpr f32 two_pi = 6.2831853f;
    constexpr f32 deg_to_rad = pi / 180.0f;
    constexpr f32 rad_to_deg = 180.0f / pi;
    // Largest representable value
    constexpr f32 largest_representable = 3.402823466e+38F;
    // Smallest number such that 1.0 + epsilon != 1.0
    constexpr f32 epsilon = 1.192092896e-07F;
    // __builtin_huge_valf is supported by all major compilers.
    constexpr f32 infinity = __builtin_huge_valf();

    // Converts degrees to radians.
    //
    constexpr f32 radians(f32 degrees) {
        return degrees * deg_to_rad;
    }

    // Converts radians to degrees.
    //
    constexpr f32 degrees(f32 radians) {
        return radians * rad_to_deg;
    }

    inline f32 pow(f32 base, f32 exp) {
        return ::powf(base, exp);
    }

    inline f32 sqrt(f32 a) {
        return ::sqrtf(a);
    }

    inline f32 inv_sqrt(f32 a) {
        return 1 / sqrt(a);
    }

    constexpr f32 sign(f32 a) {
        return static_cast<f32>((a > 0.0f) - (a < 0.0f));
    }

    inline f32 sin(f32 angle) {
        return ::sinf(angle);
    }

    inline f32 cos(f32 angle) {
        return ::cosf(angle);
    }

    inline f32 tan(f32 angle) {
        return ::tanf(angle);
    }

    template <typename T>
    constexpr T abs(T a) {
        return a < T(0) ? -a : a;
    }

    template <typename T>
    constexpr T max(T a, T b) {
        return a > b ? a : b;
    }

    template <typename T>
    constexpr T min(T a, T b) {
        return a < b ? a : b;
    }

    template <typename T>
    constexpr T clamp(T x, T lower_limit, T upper_limit) {
        return min(max(x, lower_limit), upper_limit);
    }

    inline f32 round(f32 x) {
        return ::roundf(x);
    }

    inline f32 floor(f32 x) {
        return ::floorf(x);
    }

    inline f32 ceil(f32 x) {
        return ::ceilf(x);
    }

    inline f32 fract(f32 x) {
        f32 integral_part;
        return ::modff(x, &integral_part);
    }

    constexpr f32 step_to_value(f32 current, f32 target, f32 change) {
        f32 delta = target - current;
        if (abs(delta) > change) {
            return current + sign(delta) * change;
        } else {
            return target;
        }
    }

    // lerp
    // Computes the linear interpolation between a and b for the parameter t in the interval [0, 1].
    constexpr f32 lerp(f32 const a, f32 const b, f32 const t) {
        return (1.0f - t) * a + t * b;
    }

    // smoothstep
    //
    constexpr f32 smoothstep(f32 edge0, f32 edge1, f32 x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * (3.0f - 2.0f * x);
    }

    // smootherstep
    // Improved version of smoothstep that has zero 1st and 2nd order derivatives at egde0 and edge1.
    //
    // Returns 0 if x <= edge0, 1 if x >= edge1, otherwise computes 6x^5 - 15x^4 + 10x^3
    // with x rescaled to range [0.0, 1.0].
    //
    constexpr f32 smootherstep(f32 edge0, f32 edge1, f32 x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * x * ((6.0f * x - 15.0f) * x + 10.0f);
    }

    // popcount
    // Counts the number of set bits in v.
    //
    constexpr u8 popcount(u8 v) {
        v = (v & 0x55) + ((v >> 1) & 0x55);
        v = (v & 0x33) + ((v >> 2) & 0x33);
        v = (v & 0x0F) + ((v >> 4) & 0x0F);
        return v;
    }

    constexpr u16 popcount(u16 v) {
        v = (v & 0x5555) + ((v >> 1) & 0x5555);
        v = (v & 0x3333) + ((v >> 2) & 0x3333);
        v = (v & 0x0F0F) + ((v >> 4) & 0x0F0F);
        return v * 0x0101 >> 8;
    }

    constexpr u32 popcount(u32 v) {
        v = (v & 0x55555555) + ((v >> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
        v = (v & 0x0F0F0F0F) + ((v >> 4) & 0x0F0F0F0F);
        return v * 0x01010101 >> 24;
    }

    constexpr u64 popcount(u64 v) {
        v = (v & 0x5555555555555555) + ((v >> 1) & 0x5555555555555555);
        v = (v & 0x3333333333333333) + ((v >> 2) & 0x3333333333333333);
        v = (v & 0x0F0F0F0F0F0F0F0F) + ((v >> 4) & 0x0F0F0F0F0F0F0F0F);
        return v * 0x0101010101010101 >> 56;
    }

    // clz
    // Counts leading zeros.
    //
    constexpr u8 clz(u8 v) {
        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        return 8 - popcount(v);
    }

    constexpr u16 clz(u16 v) {
        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        return 16 - popcount(v);
    }

    constexpr u32 clz(u32 v) {
        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);
        return 32 - popcount(v);
    }

    constexpr u64 clz(u64 v) {
        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);
        v |= (v >> 32);
        return 64 - popcount(v);
    }

    // ilog2
    // Computes the floor of logarithm base 2 of v.
    // Returns 0 for ilog2(0).
    //
    constexpr u32 ilog2(u32 const v) {
        return 32 - clz(v);
    }

    constexpr u64 ilog2(u64 const v) {
        return 64 - clz(v);
    }

    // ilog10
    // Computes the floor of logarithm base 10 of v.
    // Returns 0 for ilog10(0).
    //
    constexpr u64 ilog10(u64 const v) {
        constexpr u64 powers_of_10[] = {1ULL,
                                        10ULL,
                                        100ULL,
                                        1000ULL,
                                        10000ULL,
                                        100000ULL,
                                        1000000ULL,
                                        10000000ULL,
                                        100000000ULL,
                                        1000000000ULL,
                                        10000000000ULL,
                                        100000000000ULL,
                                        1000000000000ULL,
                                        10000000000000ULL,
                                        100000000000000ULL,
                                        1000000000000000ULL,
                                        10000000000000000ULL,
                                        100000000000000000ULL,
                                        1000000000000000000ULL,
                                        10000000000000000000ULL};
        u64 const temp = (ilog2(v) + 1) * 1233 >> 12;
        return temp - (v < powers_of_10[temp]);
    }

    // u64 ilog10(u64 const v) {
    //     if (v < 10)
    //         return 1;
    //     if (v < 100)
    //         return 2;
    //     if (v < 1000)
    //         return 3;
    //     if (v < 10000)
    //         return 4;
    //     if (v < 100000)
    //         return 5;
    //     if (v < 1000000)
    //         return 6;
    //     if (v < 10000000)
    //         return 7;
    //     if (v < 100000000)
    //         return 8;
    //     if (v < 1000000000)
    //         return 9;
    //     if (v < 10000000000)
    //         return 10;
    //     if (v < 100000000000)
    //         return 11;
    //     if (v < 1000000000000)
    //         return 12;
    //     if (v < 10000000000000)
    //         return 13;
    //     if (v < 100000000000000)
    //         return 14;
    //     if (v < 1000000000000000)
    //         return 15;
    //     if (v < 10000000000000000)
    //         return 16;
    //     if (v < 100000000000000000)
    //         return 17;
    //     if (v < 1000000000000000000)
    //         return 18;
    //     if (v < 10000000000000000000)
    //         return 19;
    //     return 20;
    // }

} // namespace anton_engine::math
#endif // !CORE_MATH_MATH_HPP_INCLUDE
