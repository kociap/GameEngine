#ifndef ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE

#include "input/axis_action_names.hpp"
#include "key.hpp"
#include <cstdint>
#include <deque>
#include <vector>

namespace Input {
    struct Action_Binding {
        Input_Action action;
        Key key;
        float scale;

        Action_Binding(Input_Action a, Key k, float s) : action(a), key(k), scale(s) {}
    };

    struct Axis_Binding {
        Input_Axis axis;
        Key key;

        /*float sensitivity = 0.0f;
        float gravity = 0.0f;*/

        float scale;
        float raw_value = 0.0f;

        Axis_Binding(Input_Axis a, Key k, float s) : axis(a), key(k), scale(s) {}
    };

    struct Event {
        Key key;
        float value;

        Event(Key k, float v) : key(k), value(v) {}
    };

    struct Mouse_Event {
        float mouse_x;
        float mouse_y;
        float wheel;

        Mouse_Event(float m_x = 0.0f, float m_y = 0.0f, float w = 0.0f) : mouse_x(m_x), mouse_y(m_y), wheel(w) {}
    };

    struct Gamepad_Event {
        int32_t pad_index;
        Key key;
		float value;

        Gamepad_Event(int32_t pad_inx, Key k, float val) : pad_index(pad_inx), key(k), value(val) {}
    };

    class Manager {
    public:
        struct Axis {
            Input_Axis axis;
            float value = 0.0f;
            float raw_value = 0.0f;
            float scale = 0.0f;
            // If raw axis value changes sign, should we reset to 0 or continue from current value?
            int8_t snap : 1;

            Axis(Input_Axis a) : axis(a), snap(0) {}
        };

        struct Action {
            Input_Action action;
            float value = 0.0f;
            float raw_value = 0.0f;
            float scale = 0.0f;

            Action(Input_Action a) : action(a) {}
        };

        std::vector<Event> input_event_queue;
        std::vector<Mouse_Event> mouse_event_queue;
        std::vector<Gamepad_Event> gamepad_event_queue;
        std::vector<Gamepad_Event> gamepad_stick_event_queue;
        std::vector<Axis_Binding> axis_bindings;
        std::vector<Action_Binding> action_bindings;
        std::vector<Axis> axes;
        std::vector<Action> actions;

		// Use radial dead zone for gamepad sticks?
		// Turned on by default
		// If off, axial dead zone will be used instead
        uint8_t gamepad_sticks_radial_dead_zone : 1;

		// TODO TEMPORARY hardcoded deadzone for gamepad sticks
        float gamepad_dead_zone = 0.25f;

		Manager(): gamepad_sticks_radial_dead_zone(true) {}

        void load_bindings();
        void save_bindings();

        void register_axis_bindings(std::vector<Axis_Binding> const&);
        void register_action_bindings(std::vector<Action_Binding> const&);

        void process_events();

    private:
        void process_mouse_events();
        void process_gamepad_events();
    };
} // namespace Input

#endif // !ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE