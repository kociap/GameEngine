#ifndef CORE_MATH_MATH_HPP_INCLUDE
#define CORE_MATH_MATH_HPP_INCLUDE

namespace math {
	template<typename T>
    struct constants;

	template<>
	struct constants<float> {
        constexpr static float pi = 3.14159265f;
        constexpr static float deg_to_rad = pi / 180.0f;
        constexpr static float rad_to_deg = 180.0f / pi;
	};

	template<>
	struct constants<double> {
        constexpr static double pi = 3.141592653589793;
        constexpr static double deg_to_rad = pi / 180.0;
        constexpr static double rad_to_deg = 180.0 / pi;
	};

	float inv_sqrt(float number);
}

#endif // !CORE_MATH_MATH_HPP_INCLUDE
