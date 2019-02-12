#include "input/input.hpp"

#include "engine.hpp"
#include "input/input_core.hpp"

float Input::get_axis(Input_Axis axis_name) {
    /*Input_Manager& input_manager = Engine::get_input_manager();
    for (Input_Manager::Axis& axis : input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.value;
        }
    }
    return 0;*/
    return get_axis_raw(axis_name);
}

float Input::get_axis_raw(Input_Axis axis_name) {
    Input_Manager& input_manager = Engine::get_input_manager();
    for (Input_Manager::Axis& axis : input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.raw_value;
        }
    }
    return 0;
}

float Input::get_action(Input_Action action_name) {
    Input_Manager& input_manager = Engine::get_input_manager();
    for (Input_Manager::Action& action : input_manager.actions) {
        if (action_name == action.action) {
            return action.value;
        }
    }
    return 0;
}