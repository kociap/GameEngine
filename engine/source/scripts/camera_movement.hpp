#include "components/behaviour_component.hpp"
#include "components/camera.hpp"
#include "game_object.hpp"
#include "input/input.hpp"
#include "math/math.hpp"
#include "math/transform.hpp"

class Camera_Movement : public Behaviour_Component {
public:
    using Behaviour_Component::Behaviour_Component;

    Vector3 camera_side = Vector3::right;

    void update() override {
        Camera& camera = game_object.get_component<Camera>();
        Transform& transform = get_transform();

        // Look around
        float horizontal_rotation = Input::get_axis(Input_Axis::mouse_x);
        float vertical_rotation = Input::get_axis(Input_Axis::mouse_y);
        transform.rotate(Vector3::up, math::radians(-horizontal_rotation));
        camera_side = Vector3(Vector4(camera_side) * transform::rotate_y(math::radians(horizontal_rotation)));
        transform.rotate(camera_side, math::radians(vertical_rotation));

        // Move
        Vector3 camera_front = camera.get_front();
        float camera_speed = 0.15f;
        float forward = Input::get_axis(Input_Axis::move_forward);
        transform.translate(camera_front * camera_speed * forward);
        float sideways = Input::get_axis(Input_Axis::move_sideways);
        transform.translate(camera_side * camera_speed * sideways);
        float vertical = Input::get_axis(Input_Axis::move_vertical);
        transform.translate(Vector3::up * camera_speed * vertical);
    }
};