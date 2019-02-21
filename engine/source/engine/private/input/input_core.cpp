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

// DEBUG TODO remove
#include <iostream>

namespace Input {
    static void extract_bindings(std::string const& str, std::vector<Axis_Binding>& axis_bindings, std::vector<Action_Binding>& action_bindings) {
        // clang-format off
        std::unordered_map<std::string, Input_Axis> string_to_input_axis({
            {"move_forward", Input_Axis::move_forward},
            {"move_sideways", Input_Axis::move_sideways},
            {"move_vertical", Input_Axis::move_vertical},
            {"mouse_x", Input_Axis::mouse_x},
            {"mouse_y", Input_Axis::mouse_y}
        });

        std::unordered_map<std::string, Input_Action> string_to_input_action;
        // clang-format on

        struct Binding {
            std::string name;
            std::string key;
            float scale;

            Binding(std::string const& n, std::string const& k, float s) : name(n), key(k), scale(s) {}
        };

        auto parse_input_string = [](std::string const& str, std::vector<Binding>& axes, std::vector<Binding>& actions) {
            // Why is this function ugly?

            auto find_property = [](auto& properties, auto predicate) -> std::vector<utils::xml::Tag_Property>::iterator {
                auto end = properties.end();
                for (auto iter = properties.begin(); iter != end; ++iter) {
                    if (predicate(*iter)) {
                        return iter;
                    }
                }
                return end;
            };

            std::vector<utils::xml::Tag> tags(utils::xml::parse(str));
            for (utils::xml::Tag& tag : tags) {
                GE_assert(tag.name == "axis" || tag.name == "action", "Unknown tag, skipping"); // TODO replace with logging

                auto axis_prop = find_property(tag.properties, [](auto& property) { return property.name == "axis"; });
                auto action_prop = find_property(tag.properties, [](auto& property) { return property.name == "action"; });
                auto key_prop = find_property(tag.properties, [](auto& property) { return property.name == "key"; });
                auto scale_prop = find_property(tag.properties, [](auto& property) { return property.name == "scale"; });

                // TODO replace with logging
                GE_assert(axis_prop != tag.properties.end() || action_prop != tag.properties.end(), "Missing action/axis property");
                GE_assert(key_prop != tag.properties.end(), "Missing key property");
                GE_assert(scale_prop != tag.properties.end(), "Missing scale property");

                if (axis_prop != tag.properties.end()) {
                    axes.emplace_back(axis_prop->value, key_prop->value, std::stof(scale_prop->value));
                } else {
                    actions.emplace_back(action_prop->value, key_prop->value, std::stof(scale_prop->value));
                }
            }
        };

        std::vector<Binding> axes;
        std::vector<Binding> actions;
        parse_input_string(str, axes, actions);

        for (Binding& axis : axes) {
            auto key = key_from_string(axis.key);
            auto name_iter = string_to_input_axis.find(axis.name);
            GE_assert(name_iter != string_to_input_axis.end(),
                      "Attepmting to load unknown axis binding, skipping"); // TODO replace with logging
            if (name_iter != string_to_input_axis.end()) {
                axis_bindings.emplace_back(name_iter->second, key, axis.scale);
            }
        }

        for (Binding& action : actions) {
            auto key = key_from_string(action.key);
            auto name_iter = string_to_input_action.find(action.name);
            GE_assert(name_iter != string_to_input_action.end(),
                      "Attepmting to load unknown axis binding, skipping"); // TODO replace with logging
            if (name_iter != string_to_input_action.end()) {
                action_bindings.emplace_back(name_iter->second, key, action.scale);
            }
        }
    }

    void Manager::load_bindings() {
        std::string config_file;
        std::filesystem::path bindings_file_path(utils::concat_paths(Assets::current_path(), "input_bindings.config"));
        Assets::read_file_raw(bindings_file_path, config_file);
        std::vector<Axis_Binding> axes;
        std::vector<Action_Binding> actions;
        extract_bindings(config_file, axes, actions);
        register_axis_bindings(axes);
        register_action_bindings(actions);
    }

    void Manager::save_bindings() {}

    void Manager::register_axis_bindings(std::vector<Axis_Binding> const& bindings) {
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

    void Manager::register_action_bindings(std::vector<Action_Binding> const& bindings) {
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
        // TODO add actions processing

        process_mouse_events();
        process_gamepad_events();

        for (Event& event : input_event_queue) {
            // DEBUG TODO remove
            //std::cout << key_to_string(event.key) << ": " << event.value << "\n";

            for (auto& binding : axis_bindings) {
                if (binding.key == event.key) {
                    binding.raw_value = event.value;
                }
            }

            for (auto& binding : action_bindings) {
                if (binding.key == event.key) {
                }
            }
        }
        input_event_queue.clear();

        // TODO different behaviour for gamepad
        for (Axis& axis : axes) {
            float raw_value = 0.0f;
            float max_scale = 0.0f;
            bool has_mouse_bound = false;
            bool has_gamepad_axes_bound = false;
            for (auto& axis_binding : axis_bindings) {
                if (axis.axis == axis_binding.axis) {
                    if (axis_binding.key == Key::mouse_x || axis_binding.key == Key::mouse_y || axis_binding.key == Key::mouse_scroll) {
                        has_mouse_bound = true;
                    } else if (axis_binding.key == Key::gamepad_right_stick_x_axis || axis_binding.key == Key::gamepad_right_stick_y_axis ||
                               axis_binding.key == Key::gamepad_left_stick_x_axis || axis_binding.key == Key::gamepad_left_stick_y_axis ||
                               axis_binding.key == Key::gamepad_left_trigger || axis_binding.key == Key::gamepad_right_trigger) {
                        has_gamepad_axes_bound = true;
                    }

                    if (axis_binding.raw_value != 0) {
                        raw_value += axis_binding.raw_value * math::sign(axis_binding.scale);
                        max_scale = math::max(max_scale, math::abs(axis_binding.scale));
                    }
                }
            }

            if (has_mouse_bound || has_gamepad_axes_bound) {
                // Do not normalize or smooth mouse values
                axis.raw_value = raw_value;
                axis.value = raw_value * max_scale;
            } else {
                // Keep normalized and smoothed
                axis.raw_value = math::max(-1.0f, math::min(1.0f, raw_value));
                axis.scale = max_scale == 0 ? axis.scale : max_scale;

                if (axis.snap && math::sign(axis.raw_value) != math::sign(axis.value)) {
                    axis.value = 0;
                }

                float axis_value_delta = axis.scale * Time::get_delta_time();
                axis.value = math::step_to_value(axis.value, axis.raw_value, axis_value_delta);
            }
        }

        // DEBUG TODO remove
        /*std::unordered_map<Input_Axis, std::string> axis_to_string({{Input_Axis::move_forward, "move_forward"},
                                                                    {Input_Axis::move_sideways, "move_sideways"},
                                                                    {Input_Axis::move_vertical, "move_vertical"},
                                                                    {Input_Axis::mouse_x, "mouse_x"},
                                                                    {Input_Axis::mouse_y, "mouse_y"}});

        std::cout << "Time: " << Time::get_delta_time() << "\n";
        for (Axis& axis : axes) {
            std::cout << axis_to_string[axis.axis] << " scale: " << axis.scale << " raw: " << axis.raw_value << " value:" << axis.value << "\n";
        }*/
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
        for (auto& axis_binding : axis_bindings) {
            Key k = axis_binding.key;
            if (k == Key::mouse_x) {
                axis_binding.raw_value = current_frame_mouse.mouse_x;
            } else if (k == Key::mouse_y) {
                axis_binding.raw_value = current_frame_mouse.mouse_y;
            } else if (k == Key::mouse_scroll) {
                axis_binding.raw_value = current_frame_mouse.wheel;
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

        if (gamepad_sticks_radial_dead_zone) {
            // Radial dead zone
            float left_stick_length = math::min(gamepad_left_stick.length(), 1.0f);
            if (left_stick_length > gamepad_dead_zone) {
                gamepad_left_stick = normalize(gamepad_left_stick) * (left_stick_length - gamepad_dead_zone) / (1 - gamepad_dead_zone);
            } else {
                gamepad_left_stick = Vector2::zero;
            }

            float right_stick_length = math::min(gamepad_right_stick.length(), 1.0f);
            if (right_stick_length > gamepad_dead_zone) {
                gamepad_right_stick = normalize(gamepad_right_stick) * (right_stick_length - gamepad_dead_zone) / (1 - gamepad_dead_zone);
            } else {
                gamepad_right_stick = Vector2::zero;
            }

			//std::cout << "left_stick: " << left_stick_length << " right stick: " << right_stick_length << "\n";
        } else {
            // Axial dead zone
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
            if (binding.key == Key::gamepad_left_stick_x_axis) {
                binding.raw_value = gamepad_left_stick.x;
            } else if (binding.key == Key::gamepad_left_stick_y_axis) {
                binding.raw_value = gamepad_left_stick.y;
            } else if (binding.key == Key::gamepad_right_stick_x_axis) {
                binding.raw_value = gamepad_right_stick.x;
            } else if (binding.key == Key::gamepad_right_stick_y_axis) {
                binding.raw_value = gamepad_right_stick.y;
            }
        }

        std::cout << "gamepad_left_stick.x: " << gamepad_left_stick.x << " "
                  << "gamepad_left_stick.y: " << gamepad_left_stick.y << "\n"
                  << "gamepad_right_stick.x: " << gamepad_right_stick.x << " "
                  << "gamepad_right_stick.y: " << gamepad_right_stick.y << "\n";

        for (Gamepad_Event const& gamepad_event : gamepad_event_queue) {
            for (auto& binding : axis_bindings) {
                if (binding.key == gamepad_event.key) {
                    binding.raw_value = gamepad_event.value;
                }
            }
        }

        gamepad_stick_event_queue.clear();
        gamepad_event_queue.clear();
    }
} // namespace Input