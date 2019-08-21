#ifndef ENGINE_INPUT_INPUT_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_HPP_INCLUDE

#include <input/key_state.hpp>
#include <key.hpp>
#include <string>

namespace Input {
    struct Action_State {
        bool down = false;
        bool pressed = false;
        bool released = false;
    };

    [[nodiscard]] float get_axis(std::string const& axis);
    [[nodiscard]] float get_axis_raw(std::string const& axis);
    [[nodiscard]] Action_State get_action(std::string const& action);
    [[nodiscard]] Key_State get_key_state(Key);
    // [[nodiscard]] Any_Key_State get_any_key_state();
}; // namespace Input

#endif // !ENGINE_INPUT_INPUT_HPP_INCLUDE
