#include <engine/input.hpp>
#include <engine/input/input_internal.hpp>

#include <core/assert.hpp>
#include <core/atl/string.hpp>
#include <engine/assets.hpp>
#include <core/logging.hpp>
#include <core/math/math.hpp>
#include <core/math/vector2.hpp>
#include <core/paths.hpp>
#include <engine/time.hpp>
#include <core/utils/filesystem.hpp>
#include <core/utils/simple_xml_parser.hpp>

#include <cctype>
#include <unordered_map>

// TODO add support for multiple gamepads

namespace anton_engine::input {
    struct Action_Mapping {
        atl::String action;
        Key key;
    };

    struct Axis_Mapping {
        atl::String axis;
        Key key;
        // Scale by which to multiply raw value
        f32 raw_value_scale;
        // How fast to accumulate value in units/s
        f32 accumulation_speed;
        f32 raw_value = 0.0f;
        // Smoothed binding value
        f32 value = 0.0f;

        // If raw value changes sign, should we reset to 0 or continue from current value?
        i8 snap;
    };

    struct Event {
        Key key;
        float value;

        Event(Key k, float v): key(k), value(v) {}
    };

    struct Mouse_Event {
        float mouse_x;
        float mouse_y;
        float wheel;

        Mouse_Event(float m_x = 0.0f, float m_y = 0.0f, float w = 0.0f): mouse_x(m_x), mouse_y(m_y), wheel(w) {}
    };

    struct Gamepad_Event {
        i32 pad_index;
        Key key;
        float value;

        Gamepad_Event(i32 pad_inx, Key k, float val): pad_index(pad_inx), key(k), value(val) {}
    };

    struct Axis {
        atl::String axis;
        float value = 0.0f;
        float raw_value = 0.0f;

        Axis(atl::String_View const a): axis(a) {}
    };

    struct Action {
        atl::String action;
        Key captured_key = Key::none;
        bool down = false;
        bool pressed = false;
        bool released = false;
        bool bind_press_event = true;
        bool bind_release_event = true;

        Action(atl::String_View const a): action(a) {}
    };

    static std::unordered_map<Key, Key_State> key_states;
    // TODO redesign events
    static atl::Vector<Key> key_events_queue;
    static atl::Vector<Event> input_event_queue;
    static atl::Vector<Mouse_Event> mouse_event_queue;
    static atl::Vector<Gamepad_Event> gamepad_event_queue;
    static atl::Vector<Gamepad_Event> gamepad_stick_event_queue;

    static atl::Vector<Axis_Mapping> axis_mappings;
    static atl::Vector<Action_Mapping> action_mappings;
    static atl::Vector<Axis> axes;
    static atl::Vector<Action> actions;

    // Use radial dead zone for gamepad sticks?
    // Turned on by default
    // If off, axial dead zone will be used instead
    static bool use_radial_deadzone_for_gamepad_sticks = true;

    // TODO TEMPORARY hardcoded deadzone for gamepad sticks
    static float gamepad_dead_zone = 0.25f;

    void add_gamepad_event(i32 const pad_index, Key const k, f32 const value) {
        if (is_gamepad_stick(k)) {
            gamepad_stick_event_queue.push_back({pad_index, k, value});
        } else {
            gamepad_event_queue.push_back({pad_index, k, value});
        }

        // TODO: Differentiate between pads.
        key_events_queue.push_back(k);
    }

    void add_event(Key const k, f32 const value) {
        if (k == Key::mouse_x || k == Key::mouse_y || k == Key::mouse_scroll) {
            mouse_event_queue.push_back({value * f32(k == Key::mouse_x), value * f32(k == Key::mouse_y), value * f32(k == Key::mouse_scroll)});
        } else {
            input_event_queue.push_back({k, value});
        }
        key_events_queue.push_back(k);
    }

    static Action_Mapping const* find_mapping_with_key(atl::Vector<Action_Mapping> const& mappings, atl::String_View const action, Key key) {
        for (auto& mapping: mappings) {
            if (mapping.key == key && mapping.action == action) {
                return &mapping;
            }
        }
        return nullptr;
    }

    static void process_mouse_events() {
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

    static void process_gamepad_events() {
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
            auto apply_dead_zone = [](f32& value, f32 deadzone) -> void {
                if (math::abs(value) > deadzone) {
                    value = (value - math::sign(value) * deadzone) / (1 - deadzone);
                } else {
                    value = 0;
                }
            };

            apply_dead_zone(left_stick.x, gamepad_dead_zone);
            apply_dead_zone(left_stick.y, gamepad_dead_zone);
            apply_dead_zone(right_stick.x, gamepad_dead_zone);
            apply_dead_zone(right_stick.y, gamepad_dead_zone);
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

    void process_events() {
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
            if (is_mouse_axis(mapping.key) || is_gamepad_axis(mapping.key)) {
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

    // PUBLIC INTERFACE

    void add_axis(atl::String_View const name, Key const k, f32 const raw_value_scale, f32 const accumulation_speed, bool const snap) {
        Axis_Mapping const new_binding{name.data(), k, raw_value_scale, accumulation_speed, 0.0f, 0.0f, snap};
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

    void add_action(atl::String_View const name, Key const k) {
        Action_Mapping const new_binding{name.data(), k};
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

    f32 get_axis(atl::String_View const axis_name) {
        for (Axis& axis: axes) {
            if (axis_name == axis.axis) {
                return axis.value;
            }
        }
        ANTON_LOG_WARNING("Unknown axis " + atl::String(axis_name.data()));
        return 0;
    }

    f32 get_axis_raw(atl::String_View const axis_name) {
        for (Axis& axis: axes) {
            if (axis_name == axis.axis) {
                return axis.raw_value;
            }
        }
        ANTON_LOG_WARNING("Unknown axis " + atl::String(axis_name.data()));
        return 0;
    }

    Action_State get_action(atl::String_View const action_name) {
        for (Action& action: actions) {
            if (action_name == action.action) {
                return {action.down, action.pressed, action.released};
            }
        }
        ANTON_LOG_WARNING("Unknown action " + atl::String(action_name.data()));
        return {};
    }

    Key_State get_key_state(Key k) {
        ANTON_ASSERT(k != Key::none && k != Key::any_key, "Key may not be none or any_key");
        return key_states[k];
    }

    // Any_Key_State get_any_key_state() {
    //     Manager& input_manager = get_input_manager();
    //     return input_manager.any_key_state;
    // }
} // namespace anton_engine::input
