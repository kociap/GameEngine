#ifndef ENGINE_INPUT_KEY_STATE_HPP_INCLUDE
#define ENGINE_INPUT_KEY_STATE_HPP_INCLUDE

namespace Input {
    struct Key_State {
        double last_up_down_transition_time = 0.0f;
        float raw_value = 0.0f;
        float value = 0.0f;
        bool down = false;
        // Has been pressed or released?
        bool up_down_transitioned = false;
    };

    struct Any_Key_State {
        // Last time any key was pressed or released
        double last_up_down_transition_time = 0.0f;
        // Has any key been pressed?
        bool pressed = false;
        // Has any key been released?
        bool released = false;
        // Is any key being held down?
        bool down = false;
    };
} // namespace Input

#endif // !ENGINE_INPUT_KEY_STATE_HPP_INCLUDE
