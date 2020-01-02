#ifndef CORE_MATH_NOISE_HPP_INCLUDE
#define CORE_MATH_NOISE_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/slice.hpp>
#include <math/math.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <math/vector4.hpp>

namespace anton_engine::math {
    inline float perlin_noise(Vector2 const position) {
        static Vector2 const gradients[] = {{0.0f, 1.0f},  {0.382683f, 0.923879f},   {0.707107f, 0.707107f},   {0.923879f, 0.382683f},
                                            {1.0f, 0.0f},  {0.923879f, -0.382683f},  {0.707107f, -0.707107f},  {0.382683f, -0.923879f},
                                            {0.0f, -1.0f}, {-0.382683f, -0.923879f}, {-0.707107f, -0.707107f}, {-0.923879f, -0.382683f},
                                            {-1.0f, 0.0f}, {-0.923879f, 0.382683f},  {-0.707107f, 0.707107f},  {-0.382683f, 0.923879f}};
        static i32 const gradients_size = sizeof(gradients) / sizeof(Vector2);

        static i32 const perm_table[] = {
            11,  3,   299, 83,  42,  81,  213, 25,  98,  279, 292, 43,  22,  247, 243, 145, 245, 240, 96,  17,  26,  152, 244, 32,  62,  24,  119, 186,
            274, 188, 163, 39,  175, 90,  156, 79,  278, 237, 157, 13,  258, 7,   225, 131, 252, 94,  176, 52,  44,  136, 283, 0,   168, 208, 167, 95,
            197, 255, 259, 2,   248, 86,  271, 92,  64,  142, 234, 179, 15,  211, 132, 189, 20,  124, 35,  183, 146, 199, 294, 241, 264, 233, 57,  121,
            220, 105, 184, 260, 99,  205, 185, 36,  174, 56,  246, 269, 88,  215, 198, 204, 238, 149, 228, 63,  139, 137, 242, 84,  19,  221, 129, 66,
            103, 253, 37,  102, 256, 71,  77,  268, 135, 254, 158, 277, 126, 10,  227, 45,  100, 68,  38,  284, 14,  166, 218, 162, 216, 58,  122, 108,
            267, 69,  200, 75,  140, 117, 111, 222, 193, 55,  224, 266, 251, 257, 116, 143, 235, 114, 287, 70,  61,  209, 87,  91,  51,  276, 50,  177,
            33,  285, 291, 78,  239, 159, 262, 296, 273, 101, 125, 160, 210, 8,   4,   29,  281, 250, 232, 93,  59,  236, 54,  173, 144, 249, 169, 82,
            23,  60,  161, 9,   231, 74,  182, 223, 27,  134, 138, 202, 31,  155, 172, 21,  280, 289, 47,  97,  297, 203, 154, 164, 118, 151, 226, 113,
            123, 207, 120, 16,  298, 48,  217, 165, 12,  288, 130, 282, 187, 40,  229, 270, 110, 72,  171, 41,  230, 65,  67,  196, 28,  148, 112, 104,
            192, 195, 1,   109, 106, 212, 219, 18,  293, 80,  73,  133, 290, 272, 261, 180, 141, 190, 286, 194, 53,  214, 147, 76,  206, 34,  191, 300,
            127, 49,  170, 275, 6,   181, 153, 265, 178, 107, 5,   263, 150, 128, 115, 85,  89,  295, 201, 30,  46};
        static i32 const perm_table_size = sizeof(perm_table) / sizeof(i32);

        i64 const x_int = (position.x < 0.0f ? i64(position.x) - 1 : i64(position.x));
        i64 const y_int = (position.y < 0.0f ? i64(position.y) - 1 : i64(position.y));

        f32 const x_fract = position.x - x_int;
        f32 const y_fract = position.y - y_int;

        Vector2 const g00 = gradients[perm_table[(x_int + perm_table[y_int % perm_table_size]) % perm_table_size] % gradients_size];
        Vector2 const g10 = gradients[perm_table[(x_int + 1 + perm_table[y_int % perm_table_size]) % perm_table_size] % gradients_size];
        Vector2 const g01 = gradients[perm_table[(x_int + perm_table[(y_int + 1) % perm_table_size]) % perm_table_size] % gradients_size];
        Vector2 const g11 = gradients[perm_table[(x_int + 1 + perm_table[(y_int + 1) % perm_table_size]) % perm_table_size] % gradients_size];

        f32 const fac00 = dot(g00, {x_fract, y_fract});
        f32 const fac10 = dot(g10, {x_fract - 1.0f, y_fract});
        f32 const fac01 = dot(g01, {x_fract, y_fract - 1.0f});
        f32 const fac11 = dot(g11, {x_fract - 1.0f, y_fract - 1.0f});

        f32 const x_fac = x_fract * x_fract * x_fract * (x_fract * (x_fract * 6.0f - 15.0f) + 10.0f);
        f32 const y_fac = y_fract * y_fract * y_fract * (y_fract * (y_fract * 6.0f - 15.0f) + 10.0f);
        f32 const lerped_x0 = lerp(fac00, fac10, x_fac);
        f32 const lerped_x1 = lerp(fac01, fac11, x_fac);
        f32 const noise = lerp(lerped_x0, lerped_x1, y_fac);
        // 2D perlin returns [-sqrt(0.5), sqrt(0.5)]. Rescale to [-1, 1].
        return 1.4142135f * noise;
    }

    inline float perlin_noise(Vector2 const position, anton_stl::Slice<Vector2 const> const gradients, anton_stl::Slice<i32 const> const permutation_table) {
        i64 const x_int = (position.x < 0.0f ? i64(position.x) - 1 : i64(position.x));
        i64 const y_int = (position.y < 0.0f ? i64(position.y) - 1 : i64(position.y));

        f32 const x_fract = position.x - x_int;
        f32 const y_fract = position.y - y_int;

        i64 const perm_size = permutation_table.size();
        Vector2 const g00 = gradients[permutation_table[(x_int + permutation_table[y_int % perm_size]) % perm_size] % gradients.size()];
        Vector2 const g10 = gradients[permutation_table[(x_int + 1 + permutation_table[y_int % perm_size]) % perm_size] % gradients.size()];
        Vector2 const g01 = gradients[permutation_table[(x_int + permutation_table[(y_int + 1) % perm_size]) % perm_size] % gradients.size()];
        Vector2 const g11 = gradients[permutation_table[(x_int + 1 + permutation_table[(y_int + 1) % perm_size]) % perm_size] % gradients.size()];

        f32 const fac00 = dot(g00, {x_fract, y_fract});
        f32 const fac10 = dot(g10, {x_fract - 1.0f, y_fract});
        f32 const fac01 = dot(g01, {x_fract, y_fract - 1.0f});
        f32 const fac11 = dot(g11, {x_fract - 1.0f, y_fract - 1.0f});

        f32 const x_fac = x_fract * x_fract * x_fract * (x_fract * (x_fract * 6.0f - 15.0f) + 10.0f);
        f32 const y_fac = y_fract * y_fract * y_fract * (y_fract * (y_fract * 6.0f - 15.0f) + 10.0f);
        f32 const lerped_x0 = lerp(fac00, fac10, x_fac);
        f32 const lerped_x1 = lerp(fac01, fac11, x_fac);
        f32 const noise = lerp(lerped_x0, lerped_x1, y_fac);
        // 2D perlin returns [-sqrt(0.5), sqrt(0.5)]. Rescale to [-1, 1].
        return 1.4142135f * noise;
    }

    // float simplex_noise();
} // namespace anton_engine::math

#endif // !CORE_MATH_NOISE_HPP_INCLUDE
