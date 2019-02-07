#ifndef ENGINE_INPUT_INPUT_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_HPP_INCLUDE

#include "axis_action_names.hpp"
#include "key.hpp"

namespace input {
    float get_axis(Input_Axis axis);
    float get_axis_raw(Input_Axis axis);
    float get_action(Input_Action action);
}; // namespace input

#endif // !ENGINE_INPUT_INPUT_HPP_INCLUDE