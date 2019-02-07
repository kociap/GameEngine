#include "input_core.hpp"

#include "assets.hpp"
#include "utils/path.hpp"

void Input_Manager::load_bindings_from_file() {
    std::string bindings;
    std::filesystem::path bindings_file_path(utils::concat_paths(Assets::current_path(), "input_bindings.config"));
    Assets::read_file_raw(bindings_file_path, bindings);
}

void Input_Manager::save_bindings_to_file() {}

void Input_Manager::register_axis_bindings(std::vector<Input_Axis_Binding> const& bindings) {
    for (Input_Axis_Binding const& new_binding : bindings) {
        bool duplicate = false;
        for (Input_Axis_Binding& axis_binding : axis_bindings) {
            duplicate = duplicate || (axis_binding.axis == new_binding.axis && axis_binding.key == new_binding.key);
        }
        if (!duplicate) {
            axis_bindings.push_back(new_binding);
        }

        duplicate = false;
        for (Axis& axis : axes) {
            duplicate = duplicate || (axis.axis == new_binding.axis);
        }
        if (!duplicate) {
            axes.emplace_back(new_binding.axis);
        }
    }
}

void Input_Manager::register_action_bindings(std::vector<Input_Action_Binding> const& bindings) {
    for (Input_Action_Binding const& new_binding : bindings) {
        bool duplicate = false;
        for (Input_Action_Binding& action_binding : action_bindings) {
            duplicate = duplicate || (action_binding.action == new_binding.action && action_binding.key == new_binding.key);
        }
        if (!duplicate) {
            action_bindings.push_back(new_binding);
        }

        duplicate = false;
        for (Action& action : actions) {
            duplicate = duplicate || (action.action == new_binding.action);
        }
        if (!duplicate) {
            actions.emplace_back(new_binding.action);
        }
    }
}

void Input_Manager::process_events() {
    for (Input_Event event : input_event_queue) {
        for (auto& binding : axis_bindings) {
            if (binding.key == event.key) {
                for (Axis& axis : axes) {
                    if (axis.axis == binding.axis) {
                        axis.raw_value = event.value;
                        break;
                    }
                }
            }
        }
    }

    input_event_queue.clear();
}