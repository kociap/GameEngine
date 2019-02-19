#ifndef ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
#define ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE

#include "base_component.hpp"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include <memory>

class Camera : public Base_Component {
public:
    enum class Projection {
        ortographic,
        perspective,
    };

public:
    // Size of the camera in ortographic projection
    float size = 5;
    // Field of view in perspective projection
    float fov = 70;
    float near_plane = 0.3f;
    float far_plane = 1000.0f;

private:
    Projection projection = Projection::perspective;
    bool active = true; // Temporarily make active by default

public:
    Camera(Entity const&);
    Camera(Camera&&) = default;
    Camera& operator=(Camera&&) = default;
    virtual ~Camera();

    // Temporarily ???
    void activate();
    void deactivate();
    bool is_active();
    void set_projection(Projection);

    Vector3 get_front();

    Matrix4 get_view_matrix();
    Matrix4 get_projection_matrix();
};

#endif // !ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
