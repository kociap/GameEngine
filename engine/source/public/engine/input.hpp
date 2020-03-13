#ifndef ENGINE_INPUT_INPUT_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/string_view.hpp>
#include <engine/key.hpp>
#include <string>

namespace anton_engine::input {
    struct Key_State {
        f64 last_up_down_transition_time = 0.0;
        f32 raw_value = 0.0f;
        f32 value = 0.0f;
        bool down = false;
        // Has been pressed or released?
        bool up_down_transitioned = false;
    };

    struct Any_Key_State {
        // Last time any key was pressed or released
        f64 last_up_down_transition_time = 0.0;
        // Has any key been pressed?
        bool pressed = false;
        // Has any key been released?
        bool released = false;
        // Is any key being held down?
        bool down = false;
    };

    struct Action_State {
        bool down = false;
        bool pressed = false;
        bool released = false;
    };

    // raw_value_scale - Scale by which to multiply raw value obtained from input devices.
    // accumulation_speed - How fast to accumulate axis value in units/s.
    // snap - If raw value changes sign, should we reset to 0 or continue from current value?
    void add_axis(atl::String_View name, Key, f32 raw_value_scale, f32 accumulation_speed, bool snap);
    void add_action(atl::String_View name, Key);

    [[nodiscard]] f32 get_axis(std::string const& axis);
    [[nodiscard]] f32 get_axis_raw(std::string const& axis);
    [[nodiscard]] Action_State get_action(std::string const& action);
    [[nodiscard]] Key_State get_key_state(Key);
    // [[nodiscard]] Any_Key_State get_any_key_state();
}; // namespace anton_engine::input

#endif // !ENGINE_INPUT_INPUT_HPP_INCLUDE
