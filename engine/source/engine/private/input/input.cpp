#include "input/input.hpp"

#include "engine.hpp"
#include "input/input_core.hpp"

float Input::get_axis(Input_Axis axis_name) {
    Input::Manager& input_manager = Engine::get_input_manager();
    for (Input::Manager::Axis& axis : input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.value;
        }
    }
    return 0;
}

float Input::get_axis_raw(Input_Axis axis_name) {
    Input::Manager& input_manager = Engine::get_input_manager();
    for (Input::Manager::Axis& axis : input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.raw_value;
        }
    }
    return 0;
}

float Input::get_action(Input_Action action_name) {
    Input::Manager& input_manager = Engine::get_input_manager();
    for (Input::Manager::Action& action : input_manager.actions) {
        if (action_name == action.action) {
            return action.value;
        }
    }
    return 0;
}