#include "components/behaviour_component.hpp"
#include "components/camera.hpp"
#include "game_object.hpp"
#include "input/input.hpp"
#include "math/math.hpp"

class Camera_Movement : public Behaviour_Component {
public:
    using Behaviour_Component::Behaviour_Component;

    void update() override {
        Camera& camera = game_object.get_component<Camera>();
        Transform& transform = get_transform();
        // Look around
        float horizontal_rotation = input::get_axis_raw(Input_Axis::mouse_x);
        transform.rotate(Vector3::up, math::radians(horizontal_rotation));

        Vector3 camera_front = camera.get_front();
        Vector3& camera_position = transform.local_position;
        // Move
        float camera_speed = 0.15f;
        float forward = input::get_axis(Input_Axis::move_forward);
        camera_position += camera_front * camera_speed * forward;
        float sideways = input::get_axis(Input_Axis::move_sideways);
        camera_position += Vector3::cross(Vector3::up, camera_front).normalize() * camera_speed * sideways;
        float vertical = input::get_axis(Input_Axis::move_vertical);
        camera_position += Vector3::up * camera_speed * vertical;
    }
};