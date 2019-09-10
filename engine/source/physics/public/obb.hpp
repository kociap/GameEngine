#ifndef PHYSICS_OBB_HPP_INCLUDE
#define PHYSICS_OBB_HPP_INCLUDE

#include <math/vector3.hpp>

namespace anton_engine {
    class OBB {
    public:
        Vector3 center;
        Vector3 local_x;
        Vector3 local_y;
        Vector3 local_z;
        Vector3 halfwidths;
    };
} // namespace anton_engine

#endif // !PHYSICS_OBB_HPP_INCLUDE
