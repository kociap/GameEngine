#ifndef ENGINE_SPOTLIGHT_LIGHT_HPP_INCLUDE
#define ENGINE_SPOTLIGHT_LIGHT_HPP_INCLUDE

#include "math/vector3.hpp"
#include "color.hpp"

class Spot_light {
public:
    Vector3 position;
    Vector3 direction;
    Color color;
    float cutoff_angle;
    float blend_angle;
	float intensity;
};

#endif // !ENGINE_SPOTLIGHT_LIGHT_HPP_INCLUDE