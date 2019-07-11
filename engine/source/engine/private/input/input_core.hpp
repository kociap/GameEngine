#ifndef ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE

#include "containers/vector.hpp"
#include "input/key_state.hpp"
#include "key.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>

// TODO add support for multiple gamepads

namespace Input {
    struct Action_Mapping {
        std::string action;
        Key key;

        Action_Mapping(std::string const& a, Key k): action(a), key(k) {}
    };

    struct Axis_Mapping {
        std::string axis;
        Key key;
        // Scale by which to multiply raw value
        float raw_value_scale;
        // How fast to accumulate value in units/s
        float accumulation_speed;
        float raw_value = 0.0f;
        // Smoothed binding value
        float value = 0.0f;

        // If raw value changes sign, should we reset to 0 or continue from current value?
        int8_t snap : 1;

        Axis_Mapping(std::string const& a, Key k, float s, float sens = 1.0f): axis(a), key(k), raw_value_scale(sens), accumulation_speed(s), snap(0) {}
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
        int32_t pad_index;
        Key key;
        float value;

        Gamepad_Event(int32_t pad_inx, Key k, float val): pad_index(pad_inx), key(k), value(val) {}
    };

    struct Axis {
        std::string axis;
        float value = 0.0f;
        float raw_value = 0.0f;

        Axis(std::string const& a): axis(a) {}
    };

    struct Action {
        std::string action;
        Key captured_key = Key::none;
        bool down = false;
        bool pressed = false;
        bool released = false;
        bool bind_press_event = true;
        bool bind_release_event = true;

        Action(std::string const& a): action(a) {}
    };

    class Manager {
    public:
        std::unordered_map<Key, Key_State> key_states;
        // TODO redesign events
        containers::Vector<Key> key_events_queue;
        containers::Vector<Event> input_event_queue;
        containers::Vector<Mouse_Event> mouse_event_queue;
        containers::Vector<Gamepad_Event> gamepad_event_queue;
        containers::Vector<Gamepad_Event> gamepad_stick_event_queue;

        containers::Vector<Axis_Mapping> axis_mappings;
        containers::Vector<Action_Mapping> action_mappings;
        containers::Vector<Axis> axes;
        containers::Vector<Action> actions;

        // Use radial dead zone for gamepad sticks?
        // Turned on by default
        // If off, axial dead zone will be used instead
        uint8_t use_radial_deadzone_for_gamepad_sticks : 1;

        // TODO TEMPORARY hardcoded deadzone for gamepad sticks
        float gamepad_dead_zone = 0.25f;

        Manager(): use_radial_deadzone_for_gamepad_sticks(true) {}

        void add_event(Gamepad_Event);
        void add_event(Event);
        void add_event(Mouse_Event);
        void add_gamepad_stick_event(Gamepad_Event);

        void load_bindings();
        void save_bindings();

        void register_axis_mappings(containers::Vector<Axis_Mapping> const&);
        void register_action_mappings(containers::Vector<Action_Mapping> const&);

        void process_events();

    private:
        void process_mouse_events();
        void process_gamepad_events();
    };
} // namespace Input

#endif // !ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE