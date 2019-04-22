#include "input/input.hpp"

#include "debug_macros.hpp"
#include "engine.hpp"
#include "input/input_core.hpp"

float Input::get_axis(std::string const& axis_name) {
    Input::Manager& input_manager = Engine::get_input_manager();
    for (Input::Manager::Axis& axis : input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.value;
        }
    }
    GE_log("Unknown axis " + axis_name);
    return 0;
}

float Input::get_axis_raw(std::string const& axis_name) {
    Input::Manager& input_manager = Engine::get_input_manager();
    for (Input::Manager::Axis& axis : input_manager.axes) {
        if (axis_name == axis.axis) {
            return axis.raw_value;
        }
    }
    GE_log("Unknown axis " + axis_name);
    return 0;
}

float Input::get_action(std::string const& action_name) {
    Input::Manager& input_manager = Engine::get_input_manager();
    for (Input::Manager::Action& action : input_manager.actions) {
        if (action_name == action.action) {
            return action.raw_value;
        }
    }
    GE_log("Unknown action " + action_name);
    return 0;
}