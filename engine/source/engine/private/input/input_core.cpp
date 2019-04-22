#include "input_core.hpp"

#include "assets.hpp"
#include "debug_macros.hpp"
#include "math/math.hpp"
#include "math/vector2.hpp"
#include "time.hpp"
#include "utils/path.hpp"
#include "utils/simple_xml_parser.hpp"

#include <cctype>
#include <unordered_map>

namespace Input {
    static bool is_not_axis(Key key) {
        return !utils::key::is_mouse_axis(key) && !utils::key::is_gamepad_axis(key);
    }

    static void extract_bindings(std::string const& str, containers::Vector<Axis_Binding>& axis_bindings, containers::Vector<Action_Binding>& action_bindings) {
        auto find_property = [](auto& properties, auto predicate) -> containers::Vector<utils::xml::Tag_Property>::iterator {
            auto end = properties.end();
            for (auto iter = properties.begin(); iter != end; ++iter) {
                if (predicate(*iter)) {
                    return iter;
                }
            }
            return end;
        };

        containers::Vector<utils::xml::Tag> tags(utils::xml::parse(str));
        for (utils::xml::Tag& tag : tags) {
            GE_assert(tag.name == "axis" || tag.name == "action", "Unknown tag, skipping"); // TODO replace with logging

            auto axis_prop = find_property(tag.properties, [](auto& property) { return property.name == "axis"; });
            auto action_prop = find_property(tag.properties, [](auto& property) { return property.name == "action"; });
            auto key_prop = find_property(tag.properties, [](auto& property) { return property.name == "key"; });
            auto scale_prop = find_property(tag.properties, [](auto& property) { return property.name == "scale"; });
            auto sensitivity_prop = find_property(tag.properties, [](auto& property) { return property.name == "sensitivity"; });

            // TODO replace with logging
            GE_assert(axis_prop != tag.properties.end() || action_prop != tag.properties.end(), "Missing action/axis property");
            GE_assert(key_prop != tag.properties.end(), "Missing key property");
            GE_assert(sensitivity_prop != tag.properties.end(), "Missing sensitivity property");

            if (axis_prop != tag.properties.end()) {
                GE_assert(scale_prop != tag.properties.end(), "Missing scale property");
                axis_bindings.emplace_back(axis_prop->value, key_from_string(key_prop->value), std::stof(scale_prop->value),
                                           std::stof(sensitivity_prop->value));
            } else {
                action_bindings.emplace_back(action_prop->value, key_from_string(key_prop->value), std::stof(sensitivity_prop->value));
            }
        }
    }

    void Manager::load_bindings() {
        std::string config_file;
        std::filesystem::path bindings_file_path(utils::concat_paths(Assets::current_path(), "input_bindings.config"));
        Assets::read_file_raw(bindings_file_path, config_file);
        containers::Vector<Axis_Binding> axes;
        containers::Vector<Action_Binding> actions;
        extract_bindings(config_file, axes, actions);
        register_axis_bindings(axes);
        register_action_bindings(actions);
    }

    void Manager::save_bindings() {}

    void Manager::register_axis_bindings(containers::Vector<Axis_Binding> const& bindings) {
        for (Axis_Binding const& new_binding : bindings) {
            bool duplicate = false;
            for (Axis_Binding& axis_binding : axis_bindings) {
                duplicate = duplicate || (axis_binding.axis == new_binding.axis && axis_binding.key == new_binding.key);
            }
            if (!duplicate) {
                axis_bindings.push_back(new_binding);
            }

            duplicate = false;
            for (Axis& axis : axes) {
                duplicate = duplicate || (axis.axis == new_binding.axis);
            }
            if (!duplicate) {
                axes.emplace_back(new_binding.axis);
            }
        }
    }

    void Manager::register_action_bindings(containers::Vector<Action_Binding> const& bindings) {
        for (Action_Binding const& new_binding : bindings) {
            bool duplicate = false;
            for (Action_Binding& action_binding : action_bindings) {
                duplicate = duplicate || (action_binding.action == new_binding.action && action_binding.key == new_binding.key);
            }
            if (!duplicate) {
                action_bindings.push_back(new_binding);
            }

            duplicate = false;
            for (Action& action : actions) {
                duplicate = duplicate || (action.action == new_binding.action);
            }
            if (!duplicate) {
                actions.emplace_back(new_binding.action);
            }
        }
    }

    void Manager::process_events() {
        // TODO add key capturing for actions

        process_mouse_events();
        process_gamepad_events();

        for (Event& event : input_event_queue) {
            for (auto& binding : axis_bindings) {
                if (binding.key == event.key || (binding.key == Key::any_key && is_not_axis(event.key))) {
                    binding.raw_value = event.value * binding.sensitivity;
                }
            }

            for (auto& binding : action_bindings) {
                if (binding.key == event.key || (binding.key == Key::any_key && is_not_axis(event.key))) {
                    binding.raw_value = event.value * binding.sensitivity;
                }
            }
        }
        input_event_queue.clear();

        float delta_time = Time::get_delta_time();
        for (auto& axis_binding : axis_bindings) {
            if (utils::key::is_mouse_axis(axis_binding.key) || utils::key::is_gamepad_axis(axis_binding.key)) {
                // Force raw for those axes
                axis_binding.value = math::sign(axis_binding.scale) * axis_binding.raw_value;
            } else {
                if (axis_binding.snap && axis_binding.raw_value != 0 && math::sign(axis_binding.value) != math::sign(axis_binding.raw_value)) {
                    axis_binding.value = 0;
                }

                float axis_value_delta = math::abs(axis_binding.scale) * delta_time;
                axis_binding.value = math::step_to_value(axis_binding.value, math::sign(axis_binding.scale) * axis_binding.raw_value, axis_value_delta);
            }
        }

        for (auto& axis : axes) {
            axis.raw_value = 0;
            axis.value = 0;
            for (auto& binding : axis_bindings) {
                if (axis.axis == binding.axis) {
                    axis.raw_value += binding.raw_value;
                    axis.value += binding.value;
                }
            }
        }

        for (auto& action : actions) {
            action.raw_value = 0;
            for (auto& binding : action_bindings) {
                if (action.action == binding.action) {
                    action.raw_value += binding.raw_value;
                }
            }
        }
    }

    void Manager::process_mouse_events() {
        Mouse_Event current_frame_mouse;

        for (Mouse_Event const& mouse_event : mouse_event_queue) {
            current_frame_mouse.mouse_x += mouse_event.mouse_x;
            current_frame_mouse.mouse_y += mouse_event.mouse_y;
            current_frame_mouse.wheel += mouse_event.wheel;
        }

        mouse_event_queue.clear();

        // Reset bindings
        for (auto& binding : axis_bindings) {
            if (binding.key == Key::mouse_x) {
                binding.raw_value = current_frame_mouse.mouse_x * binding.sensitivity;
            } else if (binding.key == Key::mouse_y) {
                binding.raw_value = current_frame_mouse.mouse_y * binding.sensitivity;
            } else if (binding.key == Key::mouse_scroll) {
                binding.raw_value = current_frame_mouse.wheel * binding.sensitivity;
            }
        }

        for (auto& binding : action_bindings) {
            if (binding.key == Key::mouse_x) {
                binding.raw_value = current_frame_mouse.mouse_x * binding.sensitivity;
            } else if (binding.key == Key::mouse_y) {
                binding.raw_value = current_frame_mouse.mouse_y * binding.sensitivity;
            } else if (binding.key == Key::mouse_scroll) {
                binding.raw_value = current_frame_mouse.wheel * binding.sensitivity;
            }
        }
    }

    void Manager::process_gamepad_events() {
        Vector2 gamepad_left_stick;
        Vector2 gamepad_right_stick;

        for (Gamepad_Event const& gamepad_stick_event : gamepad_stick_event_queue) {
            if (gamepad_stick_event.key == Key::gamepad_right_stick_x_axis) {
                gamepad_right_stick.x = gamepad_stick_event.value;
            } else if (gamepad_stick_event.key == Key::gamepad_right_stick_y_axis) {
                gamepad_right_stick.y = gamepad_stick_event.value;
            } else if (gamepad_stick_event.key == Key::gamepad_left_stick_x_axis) {
                gamepad_left_stick.x = gamepad_stick_event.value;
            } else if (gamepad_stick_event.key == Key::gamepad_left_stick_y_axis) {
                gamepad_left_stick.y = gamepad_stick_event.value;
            }
        }

        // TODO radial dead zone makes axes never reach 1 (values are slightly less than 1, e.g. 0.99996)

        if (gamepad_sticks_radial_dead_zone) { // Radial dead zone
            float left_stick_length = math::min(gamepad_left_stick.length(), 1.0f);
            if (left_stick_length > gamepad_dead_zone) {
                gamepad_left_stick = math::normalize(gamepad_left_stick) * (left_stick_length - gamepad_dead_zone) / (1 - gamepad_dead_zone);
            } else {
                gamepad_left_stick = Vector2::zero;
            }

            float right_stick_length = math::min(gamepad_right_stick.length(), 1.0f);
            if (right_stick_length > gamepad_dead_zone) {
                gamepad_right_stick = math::normalize(gamepad_right_stick) * (right_stick_length - gamepad_dead_zone) / (1 - gamepad_dead_zone);
            } else {
                gamepad_right_stick = Vector2::zero;
            }
        } else { // Axial dead zone
            auto apply_dead_zone = [this](float& value) -> void {
                if (math::abs(value) > gamepad_dead_zone) {
                    value = (value - math::sign(value) * gamepad_dead_zone) / (1 - gamepad_dead_zone);
                } else {
                    value = 0;
                }
            };

            apply_dead_zone(gamepad_left_stick.x);
            apply_dead_zone(gamepad_left_stick.y);
            apply_dead_zone(gamepad_right_stick.x);
            apply_dead_zone(gamepad_right_stick.y);
        }

        for (auto& binding : axis_bindings) {
            for (Gamepad_Event const& gamepad_event : gamepad_event_queue) {
                if (binding.key == gamepad_event.key) {
                    binding.raw_value = gamepad_event.value * binding.sensitivity;
                }
            }

            if (binding.key == Key::gamepad_left_stick_x_axis) {
                binding.raw_value = gamepad_left_stick.x * binding.sensitivity;
            } else if (binding.key == Key::gamepad_left_stick_y_axis) {
                binding.raw_value = gamepad_left_stick.y * binding.sensitivity;
            } else if (binding.key == Key::gamepad_right_stick_x_axis) {
                binding.raw_value = gamepad_right_stick.x * binding.sensitivity;
            } else if (binding.key == Key::gamepad_right_stick_y_axis) {
                binding.raw_value = gamepad_right_stick.y * binding.sensitivity;
            }
        }

        for (auto& binding : action_bindings) {
            for (Gamepad_Event const& gamepad_event : gamepad_event_queue) {
                if (binding.key == gamepad_event.key) {
                    binding.raw_value = gamepad_event.value * binding.sensitivity;
                }
            }

            if (binding.key == Key::gamepad_left_stick_x_axis) {
                binding.raw_value = gamepad_left_stick.x * binding.sensitivity;
            } else if (binding.key == Key::gamepad_left_stick_y_axis) {
                binding.raw_value = gamepad_left_stick.y * binding.sensitivity;
            } else if (binding.key == Key::gamepad_right_stick_x_axis) {
                binding.raw_value = gamepad_right_stick.x * binding.sensitivity;
            } else if (binding.key == Key::gamepad_right_stick_y_axis) {
                binding.raw_value = gamepad_right_stick.y * binding.sensitivity;
            }
        }

        gamepad_stick_event_queue.clear();
        gamepad_event_queue.clear();
    }
} // namespace Input