#ifndef ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE

#include "input/axis_action_names.hpp"
#include "key.hpp"
#include <vector>

struct Input_Action_Binding {
    Input_Action action;
    Key key;
    float scale;

    Input_Action_Binding(Input_Action a, Key k) : action(a), key(k) {}
};

struct Input_Axis_Binding {
    Input_Axis axis;
    Key key;

    /*float sensitivity = 0.0f;
    float gravity = 0.0f;*/

	float scale = 1.0f;
	float raw_value = 0.0f;

    Input_Axis_Binding(Input_Axis a, Key k) : axis(a), key(k) {}
};

struct Input_Event {
    Key key;
    float value;

    Input_Event(Key k, float v) : key(k), value(v) {}
};

class Input_Manager {
public:
    struct Axis {
        Input_Axis axis;
        float value = 0.0f;
        float raw_value = 0.0f;
        float scale = 0.0f;
        int8_t snap : 1;

        Axis(Input_Axis a) : axis(a), snap(0) {}
    };

    struct Action {
        Input_Action action;
        float value = 0.0f;
        float raw_value = 0.0f;
        float scale = 0.0f;

        Action(Input_Action a) : action(a) {}
    };

    std::vector<Input_Event> input_event_queue;
    std::vector<Input_Axis_Binding> axis_bindings;
    std::vector<Input_Action_Binding> action_bindings;
    std::vector<Axis> axes;
    std::vector<Action> actions;

    void load_bindings();
    void save_bindings();

    void register_axis_bindings(std::vector<Input_Axis_Binding> const&);
    void register_action_bindings(std::vector<Input_Action_Binding> const&);

    void process_events();
};

#endif // !ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE