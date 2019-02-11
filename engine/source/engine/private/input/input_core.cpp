#include "input_core.hpp"

#include "assets.hpp"
#include "debug_macros.hpp"
#include "utils/path.hpp"
#include <cctype>
#include <unordered_map>

static void extract_bindings(std::string const& str, std::vector<Input_Axis_Binding>& axis_bindings, std::vector<Input_Action_Binding>& action_bindings) {
	// clang-format off
    std::unordered_map<std::string, Input_Axis> string_to_input_axis({
        {"move_forward", Input_Axis::move_forward},
        {"move_sideways", Input_Axis::move_sideways},
        {"move_vertical", Input_Axis::move_vertical},
	    {"mouse_x", Input_Axis::mouse_x},
	    {"mouse_y", Input_Axis::mouse_y}
    });

	std::unordered_map<std::string, Input_Action> string_to_input_action;
    // clang-format on

    struct Binding {
        std::string name;
        std::string key;
    };

    auto parse_input_string = [](std::string const& str, std::vector<Binding>& axes, std::vector<Binding>& actions) {
        enum class State {
            new_line,
            header,
            header_end,
            binding_name,
            binding_name_whitespace,
            binding_key_begin,
            binding_key,
            binding_key_whitespace,
            invalid
        };

        State state = State::new_line;
        bool axis_binding = false;
        std::string header_value;
        for (char const& c : str) {
            if (state == State::new_line) {
                if (c == '[') {
                    state = State::header;
                    header_value = "";
                } else if (std::isalnum(c) || c == '_') {
                    GE_assert(header_value.size() > 0, "Missing header");
                    if (axis_binding) {
                        axes.emplace_back();
                        axes.back().name += c;
                    } else {
                        actions.emplace_back();
                        actions.back().name += c;
                    }
                    state = State::binding_name;
                }
            } else if (state == State::header) {
                if (!std::isspace(c)) {
                    if (c == ']') {
                        state = State::header_end;
                    } else {
                        header_value += static_cast<char>(std::tolower(c));
                    }
                }
            } else if (state == State::header_end) {
                GE_assert(header_value == "axis" || header_value == "action", "Invalid header value");
                GE_assert(std::isspace(c) || c == '\0', "Non-whitespace character after header");
                axis_binding = header_value == "axis";
                if (c == '\n') {
                    state = State::new_line;
                }
            } else if (state == State::binding_name) {
                if (c == '=') {
                    state = State::binding_key_begin;
                } else if (std::isspace(c)) {
                    state = State::binding_name_whitespace;
                } else {
                    if (axis_binding) {
                        axes.back().name += c;
                    } else {
                        actions.back().name += c;
                    }
                }
            } else if (state == State::binding_name_whitespace) {
                GE_assert(!std::isspace(c) && c != '=', "Invalid character after binding name");
                if (c == '=') {
                    state = State::binding_key_begin;
                }
            } else if (state == State::binding_key_begin) {
                GE_assert(c != '\n', "Missing binding key");
                if (!std::isspace(c)) {
                    if (axis_binding) {
                        axes.back().key += c;
                    } else {
                        actions.back().key += c;
                    }
                    state = State::binding_key;
                }
            } else if (state == State::binding_key) {
                if (c == '\n') {
                    state = State::new_line;
                } else if (std::isspace(c)) {
                    state = State::binding_key_whitespace;
                } else {
                    if (axis_binding) {
                        axes.back().key += c;
                    } else {
                        actions.back().key += c;
                    }
                }
            } else if (state == State::binding_key_whitespace) {
                GE_assert(std::isspace(c) || c == '\0', "Invalid character after binding key");
                if (c == '\n') {
                    state = State::new_line;
                }
            }
        }
    };

    std::vector<Binding> axes;
    std::vector<Binding> actions;
    parse_input_string(str, axes, actions);

    for (Binding& axis : axes) {
        auto key = key_from_string(axis.key);
        auto name_iter = string_to_input_axis.find(axis.name);
        GE_assert(name_iter != string_to_input_axis.end(),
                  "Attepmting to load unknown axis binding, skipping"); // TODO replace with logging
        if (name_iter != string_to_input_axis.end()) {
            axis_bindings.emplace_back(name_iter->second, key);
        }
    }

    for (Binding& action : actions) {
        auto key = key_from_string(action.key);
        auto name_iter = string_to_input_action.find(action.name);
        GE_assert(name_iter != string_to_input_action.end(),
                  "Attepmting to load unknown axis binding, skipping"); // TODO replace with logging
        if (name_iter != string_to_input_action.end()) {
            action_bindings.emplace_back(name_iter->second, key);
        }
    }
}

void Input_Manager::load_bindings() {
    std::string config_file;
    std::filesystem::path bindings_file_path(utils::concat_paths(Assets::current_path(), "input_bindings.config"));
    Assets::read_file_raw(bindings_file_path, config_file);
    std::vector<Input_Axis_Binding> axes;
    std::vector<Input_Action_Binding> actions;
    extract_bindings(config_file, axes, actions);
    register_axis_bindings(axes);
    register_action_bindings(actions);
}

void Input_Manager::save_bindings() {}

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

#include <iostream>

void Input_Manager::process_events() {
    for (Input_Event event : input_event_queue) {
        std::cout << key_to_string(event.key) << ": " << event.value << "\n";
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