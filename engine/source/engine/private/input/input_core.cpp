#include "input_core.hpp"

#include "assets.hpp"
#include "debug_macros.hpp"
#include "utils/path.hpp"
#include <cctype>
#include <unordered_map>

static void extract_bindings(std::string const& str, std::vector<Input_Axis_Binding>& axis_bindings, std::vector<Input_Action_Binding>& action_bindings) {
    // clang-format off
    std::unordered_map<std::string, Key> string_to_key({
        {"any_key", Key::any_key},
        {"mouse_x", Key::mouse_x},
        {"mouse_y", Key::mouse_y},
        {"mouse_scroll", Key::mouse_scroll},
        {"mouse_scroll_up", Key::mouse_scroll_up},
        {"mouse_scroll_down", Key::mouse_scroll_down},
        {"mouse_0", Key::mouse_0},
        {"mouse_1", Key::mouse_1},
        {"mouse_2", Key::mouse_2},
        {"mouse_3", Key::mouse_3},
        {"mouse_4", Key::mouse_4},
        {"mouse_5", Key::mouse_5},
        {"mouse_6", Key::mouse_6},
        {"mouse_7", Key::mouse_7},
        {"escape", Key::escape},
        {"enter", Key::enter},
        {"tab", Key::tab},
        {"caps_lock", Key::caps_lock},
        {"spacebar", Key::spacebar},
        {"backspace", Key::backspace},
        {"keypad_0", Key::keypad_0},
        {"keypad_1", Key::keypad_1},
        {"keypad_2", Key::keypad_2},
        {"keypad_3", Key::keypad_3},
        {"keypad_4", Key::keypad_4},
        {"keypad_5", Key::keypad_5},
        {"keypad_6", Key::keypad_6},
        {"keypad_7", Key::keypad_7},
        {"keypad_8", Key::keypad_8},
        {"keypad_9", Key::keypad_9},
        {"keypad_slash", Key::keypad_slash},
        {"keypad_period", Key::keypad_period},
        {"keypad_asteriks", Key::keypad_asteriks},
        {"keypad_minus", Key::keypad_minus},
        {"keypad_plus", Key::keypad_plus},
        {"keypad_enter", Key::keypad_enter},
        {"left", Key::left},
        {"right", Key::right},
        {"up", Key::up},
        {"down", Key::down},
        {"insert", Key::insert},
        {"home", Key::home},
        {"page_up", Key::page_up},
        {"page_down", Key::page_down},
        {"end", Key::end},
        {"delete_key", Key::delete_key},
        {"one", Key::one},
        {"two", Key::two},
        {"three", Key::three},
        {"four", Key::four},
        {"five", Key::five},
        {"six", Key::six},
        {"seven", Key::seven},
        {"eight", Key::eight},
        {"nine", Key::nine},
        {"zero", Key::zero},
        {"minus", Key::minus},
        {"plus", Key::plus},
        {"tilde", Key::tilde},
        {"a", Key::a},
        {"b", Key::b},
        {"c", Key::c},
        {"d", Key::d},
        {"e", Key::e},
        {"f", Key::f},
        {"g", Key::g},
        {"h", Key::h},
        {"i", Key::i},
        {"j", Key::j},
        {"k", Key::k},
        {"l", Key::l},
        {"m", Key::m},
        {"n", Key::n},
        {"o", Key::o},
        {"p", Key::p},
        {"q", Key::q},
        {"r", Key::r},
        {"s", Key::s},
        {"t", Key::t},
        {"u", Key::u},
        {"v", Key::v},
        {"w", Key::w},
        {"x", Key::x},
        {"y", Key::y},
        {"z", Key::z},
        {"left_curly_bracket", Key::left_curly_bracket},
        {"right_curly_bracket", Key::right_curly_bracket},
        {"pipe", Key::pipe},
        {"colon", Key::colon},
        {"semicolon", Key::semicolon},
        {"f1", Key::f1},
        {"f2", Key::f2},
        {"f3", Key::f3},
        {"f4", Key::f4},
        {"f5", Key::f5},
        {"f6", Key::f6},
        {"f7", Key::f7},
        {"f8", Key::f8},
        {"f9", Key::f9},
        {"f10", Key::f10},
        {"f11", Key::f11},
        {"f12", Key::f12},
        {"gamepad_button_0", Key::gamepad_button_0},
        {"gamepad_button_1", Key::gamepad_button_1},
        {"gamepad_button_2", Key::gamepad_button_2},
        {"gamepad_button_3", Key::gamepad_button_3},
        {"gamepad_button_4", Key::gamepad_button_4},
        {"gamepad_button_5", Key::gamepad_button_5},
        {"gamepad_button_6", Key::gamepad_button_6},
        {"gamepad_button_7", Key::gamepad_button_7},
        {"gamepad_button_8", Key::gamepad_button_8},
        {"gamepad_button_9", Key::gamepad_button_9},
        {"gamepad_button_10", Key::gamepad_button_10},
        {"gamepad_button_11", Key::gamepad_button_11},
        {"gamepad_button_12", Key::gamepad_button_12},
        {"gamepad_button_13", Key::gamepad_button_13},
        {"gamepad_button_14", Key::gamepad_button_14},
        {"gamepad_button_15", Key::gamepad_button_15},
        {"gamepad_button_16", Key::gamepad_button_16},
        {"gamepad_button_17", Key::gamepad_button_17},
        {"gamepad_button_18", Key::gamepad_button_18},
        {"gamepad_button_19", Key::gamepad_button_19}
    });

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
        auto key_iter = string_to_key.find(axis.key);
        auto name_iter = string_to_input_axis.find(axis.name);
        GE_assert(key_iter != string_to_key.end() && name_iter != string_to_input_axis.end(),
                  "Attepmting to load unknown axis binding, skipping"); // TODO replace with logging
        if (key_iter != string_to_key.end() && name_iter != string_to_input_axis.end()) {
            axis_bindings.emplace_back(name_iter->second, key_iter->second);
        }
    }

    for (Binding& action : actions) {
        auto key_iter = string_to_key.find(action.key);
        auto name_iter = string_to_input_action.find(action.name);
        GE_assert(key_iter != string_to_key.end() && name_iter != string_to_input_action.end(),
                  "Attepmting to load unknown axis binding, skipping"); // TODO replace with logging
        if (key_iter != string_to_key.end() && name_iter != string_to_input_action.end()) {
            action_bindings.emplace_back(name_iter->second, key_iter->second);
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