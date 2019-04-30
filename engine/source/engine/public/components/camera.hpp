#ifndef ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
#define ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE

#include "components/transform.hpp"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"

class Camera {
public:
    static Vector3 get_top(Transform&);
    static Vector3 get_front(Transform&);
    static Matrix4 get_view_matrix(Transform&);
    static Matrix4 get_projection_matrix(Camera&, Transform&);

    // Size of the camera in ortographic projection
    // float size = 5;
    // Field of view in perspective projection
    float fov = 70;
    float near_plane = 0.3f;
    float far_plane = 1000.0f;
    bool active = true; // Temporarily make active by default
};

#endif // !ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
