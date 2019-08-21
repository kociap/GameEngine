#ifndef PHYSICS_OBB_HPP_INCLUDE
#define PHYSICS_OBB_HPP_INCLUDE

#include <math/vector3.hpp>

class OBB {
public:
    Vector3 center;
    Vector3 local_x;
    Vector3 local_y;
    Vector3 local_z;
    Vector3 halfwidths;
};

#endif // !PHYSICS_OBB_HPP_INCLUDE
