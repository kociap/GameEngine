#ifndef ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE

#include "axis_action_names.hpp"
#include "key.hpp"
#include <queue>
#include <vector>

struct Input_Binding {};

struct Input_Action_Binding : public Input_Binding {
    Input_Action action;
    Key key;

    Input_Action_Binding(Input_Action a, Key k) : action(a), key(k) {}
};

struct Input_Axis_Binding : public Input_Binding {
    Input_Axis axis;
    Key key;

    Input_Axis_Binding(Input_Axis a, Key k) : axis(a), key(k) {}
};

struct Input_Event {
    Key key;
    float value;
};

struct Input_Manager {
    struct Axis {
        Input_Axis axis;
        float raw_value;
        float value;

        Axis(Input_Axis a) : axis(a), raw_value(0), value(0) {}
    };

    struct Action {
        Input_Action action;
        float value;
    };

    std::vector<Input_Event> input_event_queue;
    std::vector<Input_Axis_Binding> axis_bindings;
    std::vector<Input_Action_Binding> action_bindings;
    std::vector<Axis> axes;
    std::vector<Action> actions;

    void init();
    void shutdown();

    void register_axis_bindings(std::vector<Input_Axis_Binding> const&);
    void register_action_bindings(std::vector<Input_Action_Binding> const&);

    void process_events();
};

#endif // !ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE