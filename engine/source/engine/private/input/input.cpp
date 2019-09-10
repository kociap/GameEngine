#include <input/input.hpp>

#include <anton_assert.hpp>
#include <input/input_core.hpp>
#include <logging.hpp>
#include <anton_stl/string.hpp>

namespace anton_engine::Input {
    float get_axis(std::string const& axis_name) {
        Manager& input_manager = get_input_manager();
        for (Axis& axis: input_manager.axes) {
            if (axis_name == axis.axis) {
                return axis.value;
            }
        }
        ANTON_LOG_WARNING("Unknown axis " + anton_stl::String(axis_name.data()));
        return 0;
    }

    float get_axis_raw(std::string const& axis_name) {
        Manager& input_manager = get_input_manager();
        for (Axis& axis: input_manager.axes) {
            if (axis_name == axis.axis) {
                return axis.raw_value;
            }
        }
        ANTON_LOG_WARNING("Unknown axis " + anton_stl::String(axis_name.data()));
        return 0;
    }

    Action_State get_action(std::string const& action_name) {
        Manager& input_manager = get_input_manager();
        for (Action& action: input_manager.actions) {
            if (action_name == action.action) {
                return {action.down, action.pressed, action.released};
            }
        }
        ANTON_LOG_WARNING("Unknown action " + anton_stl::String(action_name.data()));
        return {};
    }

    Key_State get_key_state(Key k) {
        ANTON_ASSERT(k != Key::none && k != Key::any_key, "Key may not be none or any_key");
        Manager& input_manager = get_input_manager();
        return input_manager.key_states[k];
    }

    // Any_Key_State get_any_key_state() {
    //     Manager& input_manager = get_input_manager();
    //     return input_manager.any_key_state;
    // }
} // namespace anton_engine::Input
