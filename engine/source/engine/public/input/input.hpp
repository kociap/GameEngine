#ifndef ENGINE_INPUT_INPUT_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_HPP_INCLUDE

#include "axis_action_names.hpp"
#include "key.hpp"

struct Input {
    static float get_axis(Input_Axis axis);
    static float get_axis_raw(Input_Axis axis);
    static float get_action(Input_Action action);
};

#endif // !ENGINE_INPUT_INPUT_HPP_INCLUDE