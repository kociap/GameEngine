#include <core/random.hpp>

#include <core/assert.hpp>
#include <core/atl/type_traits.hpp>

#include <random>

namespace anton_engine {
    static std::mt19937_64 mersenne_engine(278432434351ULL);

    template <typename T>
    auto subtract_as_unsigned(T x, T y) -> atl::make_unsigned<T> {
        if constexpr (atl::is_unsigned<T>) {
            return x - y;
        } else {
            if (x >= 0 && y < 0) {
                return T(x) + T(-(y + 1)) + 1;
            } else {
                return T(x - y);
            }
        }
    }

    i64 random_i64(i64 min, i64 max) {
        u64 const engine_range = subtract_as_unsigned<u64>(mersenne_engine.max(), mersenne_engine.min());
        u64 const range = subtract_as_unsigned<i64>(max, min);
        ANTON_ASSERT(range >= 0, "max < min");

        if (range == 0) {
            return min;
        } else if (range < engine_range) {
            u64 scale = engine_range / (range + 1);
            while (true) {
                u64 const number = mersenne_engine() / scale;
                if (number <= range) {
                    if (min >= 0) {
                        return i64(number) + min;
                    } else if (number > u64(-(min + 1))) {
                        return i64(number - u64(-(min + 1)) - 1);
                    } else {
                        return i64(number) + min;
                    }
                }
            }
        } else { // range == engine_range
            u64 const number = mersenne_engine();
            if (number >= (1ULL << 63)) {
                return number - (1ULL << 63);
            } else {
                return i64(number) - i64((1ULL << 63) - 1) - 1;
            }
        }
    }

    f32 random_f32(f32 min, f32 max) {
        return std::uniform_real_distribution<f32>(min, max)(mersenne_engine);
    }

    f64 random_f64(f64 min, f64 max) {
        return std::uniform_real_distribution<f64>(min, max)(mersenne_engine);
    }

    void seed_default_random_engine(u64 const s) {
        mersenne_engine.seed(s);
    }
} // namespace anton_engine
