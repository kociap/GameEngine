#include "input.hpp"

#include "input_core.hpp"

extern Input_Manager g_input_manager;

float Input::get_axis(Input_Axis axis_name) {
    for (Input_Manager::Axis& axis : g_input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.value;
        }
    }
}

float Input::get_axis_raw(Input_Axis axis_name) {
    for (Input_Manager::Axis& axis : g_input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.raw_value;
        }
    }
}

float Input::get_action(Input_Action action_name) {
    for (Input_Manager::Action& action : g_input_manager.actions) {
        if (action_name == action.action) {
            return action.value;
        }
    }
}