#ifndef ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE

#include "input/axis_action_names.hpp"
#include "key.hpp"
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
        std::vector<Axis_Binding> axis_bindings;
        std::vector<Action_Binding> action_bindings;
        std::vector<Axis> axes;
        std::vector<Action> actions;

        // Buffer for storing values from previous frames used for mouse smoothing
        std::deque<Mouse_Event> mouse_values_buffer;

        uint32_t mouse_buffer_size_limit = 10;

        void load_bindings();
        void save_bindings();

        void register_axis_bindings(std::vector<Axis_Binding> const&);
        void register_action_bindings(std::vector<Action_Binding> const&);

        void process_events();

    private:
        void process_mouse_events();
    };
} // namespace Input

#endif // !ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE