#include <input/input_core.hpp>

#include <anton_assert.hpp>
#include <assets.hpp>
#include <logging.hpp>
#include <math/math.hpp>
#include <math/vector2.hpp>
#include <paths.hpp>
#include <time.hpp>
#include <utils/filesystem.hpp>
#include <utils/simple_xml_parser.hpp>

#include <cctype>
#include <unordered_map>

#include <build_config.hpp>
#if ANTON_WITH_EDITOR
#    include <editor.hpp>
#else
#    include <engine.hpp>
#endif

namespace anton_engine {
    Input::Manager& get_input_manager() {
#if ANTON_WITH_EDITOR
        return Editor::get_input_manager();
#else
        return Engine::get_input_manager();
#endif
    }
} // namespace anton_engine

namespace anton_engine::Input {
    static Action_Mapping const* find_mapping_with_key(anton_stl::Vector<Action_Mapping> const& mappings, std::string const& action, Key key) {
        for (auto& mapping: mappings) {
            if (mapping.key == key && mapping.action == action) {
                return &mapping;
            }
        }
        return nullptr;
    }

    static void extract_bindings(std::string const& str, anton_stl::Vector<Axis_Mapping>& axis_mappings, anton_stl::Vector<Action_Mapping>& action_mappings) {
        auto find_property = [](auto& properties, auto predicate) -> anton_stl::Vector<utils::xml::Tag_Property>::iterator {
            auto end = properties.end();
            for (auto iter = properties.begin(); iter != end; ++iter) {
                if (predicate(*iter)) {
                    return iter;
                }
            }
            return end;
        };

        anton_stl::Vector<utils::xml::Tag> tags(utils::xml::parse(str));
        for (utils::xml::Tag& tag: tags) {
            if (tag.name != "axis" && tag.name != "action") {
                ANTON_LOG_INFO("Unknown tag, skipping...");
                continue;
            }

            auto axis_prop = find_property(tag.properties, [](auto& property) { return property.name == "axis"; });
            auto action_prop = find_property(tag.properties, [](auto& property) { return property.name == "action"; });
            auto key_prop = find_property(tag.properties, [](auto& property) { return property.name == "key"; });
            auto accumulation_speed_prop = find_property(tag.properties, [](auto& property) { return property.name == "scale"; });
            auto sensitivity_prop = find_property(tag.properties, [](auto& property) { return property.name == "sensitivity"; });

            if (axis_prop == tag.properties.end() && action_prop == tag.properties.end()) {
                ANTON_LOG_INFO("Missing action/axis property, skipping...");
                continue;
            }
            if (key_prop == tag.properties.end()) {
                ANTON_LOG_INFO("Missing key property, skipping...");
                continue;
            }

            if (axis_prop != tag.properties.end()) {
                if (sensitivity_prop == tag.properties.end()) {
                    ANTON_LOG_INFO("Missing sensitivity property, skipping...");
                    continue;
                }
                if (accumulation_speed_prop == tag.properties.end()) {
                    ANTON_LOG_INFO("Missing scale property, skipping...");
                    continue;
                }
                axis_mappings.emplace_back(axis_prop->value, key_from_string(key_prop->value), std::stof(accumulation_speed_prop->value),
                                           std::stof(sensitivity_prop->value));
            } else {
                action_mappings.emplace_back(action_prop->value, key_from_string(key_prop->value));
            }
        }
    }

    void Manager::load_bindings() {
        // TODO uses engine exe dir
        std::filesystem::path bindings_file_path(utils::concat_paths(paths::engine_executable_directory(), "input_bindings.config"));
        std::string config_file = assets::read_file_raw_string(bindings_file_path);
        anton_stl::Vector<Axis_Mapping> loaded_axes;
        anton_stl::Vector<Action_Mapping> loaded_actions;
        extract_bindings(config_file, loaded_axes, loaded_actions);
        register_axis_mappings(loaded_axes);
        register_action_mappings(loaded_actions);
    }

    void Manager::save_bindings() {}

    void Manager::register_axis_mappings(anton_stl::Vector<Axis_Mapping> const& bindings) {
        for (Axis_Mapping const& new_binding: bindings) {
            bool duplicate = false;
            for (Axis_Mapping& axis_binding: axis_mappings) {
                duplicate = duplicate || (axis_binding.axis == new_binding.axis && axis_binding.key == new_binding.key);
            }
            if (!duplicate) {
                axis_mappings.push_back(new_binding);
            }

            duplicate = false;
            for (Axis& axis: axes) {
                duplicate = duplicate || (axis.axis == new_binding.axis);
            }
            if (!duplicate) {
                axes.emplace_back(new_binding.axis);
            }
        }
    }

    void Manager::register_action_mappings(anton_stl::Vector<Action_Mapping> const& bindings) {
        for (Action_Mapping const& new_binding: bindings) {
            bool duplicate = false;
            for (Action_Mapping& action_binding: action_mappings) {
                duplicate = duplicate || (action_binding.action == new_binding.action && action_binding.key == new_binding.key);
            }
            if (!duplicate) {
                action_mappings.push_back(new_binding);
            }

            duplicate = false;
            for (Action& action: actions) {
                duplicate = duplicate || (action.action == new_binding.action);
            }
            if (!duplicate) {
                actions.emplace_back(new_binding.action);
            }
        }
    }

    void Manager::add_event(Gamepad_Event e) {
        gamepad_event_queue.push_back(e);
        key_events_queue.push_back(e.key);
    }
    void Manager::add_event(Event e) {
        input_event_queue.push_back(e);
        key_events_queue.push_back(e.key);
    }
    void Manager::add_event(Mouse_Event e) {
        mouse_event_queue.push_back(e);
        if (e.mouse_x != 0.0f) {
            key_events_queue.push_back(Key::mouse_x);
        }
        if (e.mouse_y != 0.0f) {
            key_events_queue.push_back(Key::mouse_y);
        }
        if (e.wheel != 0.0f) {
            key_events_queue.push_back(Key::mouse_scroll);
        }
    }
    void Manager::add_gamepad_stick_event(Gamepad_Event e) {
        gamepad_stick_event_queue.push_back(e);
        key_events_queue.push_back(e.key);
    }

    void Manager::process_events() {
        // TODO add any_key support

        for (auto& [key, key_state]: key_states) {
            key_state.up_down_transitioned = false;
        }

        process_mouse_events();
        process_gamepad_events();

        // Update key_state
        for (auto [key, value]: input_event_queue) {
            Key_State& key_state = key_states[key];
            key_state.value = key_state.raw_value = value;
            key_state.up_down_transitioned = (value != 0.0f) != key_state.down;
            key_state.down = value != 0.0f;
        }
        input_event_queue.clear();

        float delta_time = get_delta_time();
        for (auto& mapping: axis_mappings) {
            Key_State const& key_state = key_states[mapping.key];
            if (utils::key::is_mouse_axis(mapping.key) || utils::key::is_gamepad_axis(mapping.key)) {
                // Force raw for those axes
                mapping.raw_value = key_state.value;
                mapping.value = math::sign(mapping.accumulation_speed) * mapping.raw_value_scale * key_state.value;
            } else {
                mapping.raw_value = key_state.value;
                float raw_scaled = mapping.raw_value_scale * mapping.raw_value;
                if (mapping.snap && raw_scaled != 0 && math::sign(mapping.value) != math::sign(mapping.raw_value)) {
                    mapping.value = 0;
                }

                float value_delta = math::abs(mapping.accumulation_speed) * delta_time;
                mapping.value = math::step_to_value(mapping.value, math::sign(mapping.accumulation_speed) * raw_scaled, value_delta);
            }
        }

        for (auto& axis: axes) {
            axis.raw_value = 0;
            axis.value = 0;
            for (auto& mapping: axis_mappings) {
                if (axis.axis == mapping.axis) {
                    axis.raw_value += mapping.raw_value_scale * mapping.raw_value;
                    axis.value += mapping.value;
                }
            }
        }

        for (auto& action: actions) {
            ANTON_ASSERT(action.bind_press_event || action.bind_release_event, "Action is not bound to any event");
            bool paired_action = action.bind_press_event && action.bind_release_event;
            action.down = paired_action && action.down; // If not paired, reset every time because we don't know the state
            action.pressed = false;
            action.released = false;
            for (Key const k: key_events_queue) {
                if (action.captured_key == k) {
                    // If the captured key is not none, then we have a mapping that allowed us to capture the key
                    Key_State const& key_state = key_states[k];
                    action.down = key_state.down;
                    action.pressed = key_state.up_down_transitioned && key_state.down;
                    action.released = key_state.up_down_transitioned && !key_state.down;
                    if (action.released) {
                        action.captured_key = Key::none;
                    }
                }

                // Sort of fallthrough because we might have set key to none in the previous if-clause
                if (action.captured_key == Key::none) {
                    Action_Mapping const* action_mapping = find_mapping_with_key(action_mappings, action.action, k);
                    if (action_mapping) {
                        Key_State const& key_state = key_states[k];
                        if (paired_action) {
                            action.down = key_state.down;
                            action.pressed = key_state.up_down_transitioned && key_state.down;
                            action.captured_key = k;
                        } else if (action.bind_release_event) {
                            // If another unpaired key has been released, keep the relase state
                            action.released = action.released || (key_state.up_down_transitioned && !key_state.down);
                        } else if (action.bind_press_event) {
                            // If another unpaired key has been pressed, keep the press state
                            action.down = action.down || key_state.down;
                            action.pressed = action.pressed || (key_state.up_down_transitioned && key_state.down);
                        }
                    }
                }
            }
        }
        key_events_queue.clear();
    }

    void Manager::process_mouse_events() {
        Mouse_Event current_frame_mouse;
        for (auto [mouse_x, mouse_y, wheel]: mouse_event_queue) {
            current_frame_mouse.mouse_x += mouse_x;
            current_frame_mouse.mouse_y += mouse_y;
            current_frame_mouse.wheel += wheel;
        }
        mouse_event_queue.clear();

        key_states[Key::mouse_x].value = key_states[Key::mouse_x].raw_value = current_frame_mouse.mouse_x;
        key_states[Key::mouse_y].value = key_states[Key::mouse_y].raw_value = current_frame_mouse.mouse_y;
        key_states[Key::mouse_scroll].value = key_states[Key::mouse_scroll].raw_value = current_frame_mouse.wheel;
    }

    void Manager::process_gamepad_events() {
        Vector2 left_stick;
        Vector2 right_stick;

        Key_State& left_stick_x_state = key_states[Key::gamepad_left_stick_x_axis];
        Key_State& left_stick_y_state = key_states[Key::gamepad_left_stick_y_axis];
        Key_State& right_stick_x_state = key_states[Key::gamepad_right_stick_x_axis];
        Key_State& right_stick_y_state = key_states[Key::gamepad_right_stick_y_axis];

        for (Gamepad_Event const& stick_event: gamepad_stick_event_queue) {
            if (stick_event.key == Key::gamepad_right_stick_x_axis) {
                right_stick.x = stick_event.value;
                right_stick_x_state.raw_value = stick_event.value;
            } else if (stick_event.key == Key::gamepad_right_stick_y_axis) {
                right_stick.y = stick_event.value;
                right_stick_y_state.raw_value = stick_event.value;
            } else if (stick_event.key == Key::gamepad_left_stick_x_axis) {
                left_stick.x = stick_event.value;
                left_stick_x_state.raw_value = stick_event.value;
            } else if (stick_event.key == Key::gamepad_left_stick_y_axis) {
                left_stick.y = stick_event.value;
                left_stick_y_state.raw_value = stick_event.value;
            }
        }

        // TODO radial dead zone makes axes never reach 1 (values are slightly less than 1, e.g. 0.99996)

        if (use_radial_deadzone_for_gamepad_sticks) { // Radial dead zone
            float left_stick_length = math::min(math::length(left_stick), 1.0f);
            if (left_stick_length > gamepad_dead_zone) {
                left_stick = math::normalize(left_stick) * (left_stick_length - gamepad_dead_zone) / (1 - gamepad_dead_zone);
            } else {
                left_stick = Vector2::zero;
            }

            float right_stick_length = math::min(math::length(right_stick), 1.0f);
            if (right_stick_length > gamepad_dead_zone) {
                right_stick = math::normalize(right_stick) * (right_stick_length - gamepad_dead_zone) / (1 - gamepad_dead_zone);
            } else {
                right_stick = Vector2::zero;
            }
        } else { // Axial dead zone
            auto apply_dead_zone = [this](float& value) -> void {
                if (math::abs(value) > gamepad_dead_zone) {
                    value = (value - math::sign(value) * gamepad_dead_zone) / (1 - gamepad_dead_zone);
                } else {
                    value = 0;
                }
            };

            apply_dead_zone(left_stick.x);
            apply_dead_zone(left_stick.y);
            apply_dead_zone(right_stick.x);
            apply_dead_zone(right_stick.y);
        }

        left_stick_x_state.value = left_stick.x;
        left_stick_y_state.value = left_stick.y;
        right_stick_x_state.value = right_stick.x;
        right_stick_y_state.value = right_stick.y;

        for (auto [pad_index, key, value]: gamepad_event_queue) {
            Key_State& key_state = key_states[key];
            key_state.value = key_state.raw_value = value;
            key_state.down = value != 0.0f;
            key_state.up_down_transitioned = true;
        }

        gamepad_stick_event_queue.clear();
        gamepad_event_queue.clear();
    }
} // namespace anton_engine::Input
