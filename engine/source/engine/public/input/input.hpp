#ifndef ENGINE_INPUT_INPUT_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_HPP_INCLUDE

#include "axis_action_names.hpp"
#include "key.hpp"

struct Input {
    static float get_axis(Input_Axis axis);
    static float get_axis_raw(Input_Axis axis);
    static float get_action(Input_Action action);

    static void add_axis_binding(Input_Axis axis, Key key);
    static void add_action_binding(Input_Action action, Key key);
};

#endif // !ENGINE_INPUT_INPUT_HPP_INCLUDE