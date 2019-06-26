#ifndef ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
#define ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE

#include <class_macros.hpp>
#include <components/transform.hpp>
#include <math/matrix4.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <serialization.hpp>

#include <cstdint>

class COMPONENT Camera {
public:
    // Size of the camera in ortographic projection
    // float size = 5;
    // Field of view in perspective projection
    float fov = 70;
    float near_plane = 0.3f;
    float far_plane = 1000.0f;
    bool active = true; // Temporarily make active by default
};

Vector3 get_camera_top(Transform&);
Vector3 get_camera_front(Transform&);
Matrix4 get_camera_view_matrix(Transform&);
Matrix4 get_camera_projection_matrix(Camera&, uint32_t viewport_width, uint32_t viewport_height);
Vector3 screen_to_world_point(Matrix4 inv_view, Matrix4 inv_projection, uint32_t screen_width, uint32_t screen_height, Vector2 point);

template <>
struct serialization::use_default_deserialize<Camera>: std::true_type {};

#endif // !ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
