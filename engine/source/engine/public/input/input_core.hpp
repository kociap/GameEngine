#ifndef ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE
#define ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE

#include "axis_action_names.hpp"
#include "key.hpp"
#include <queue>
#include <vector>

class GLFWwindow;

struct Input_Binding {};

struct Input_Action_Binding : public Input_Binding {
    Input_Action action;
    Key key;
    float axis_value = 0;

    Input_Action_Binding(Input_Action a, Key k) : action(a), key(k) {}
};

struct Input_Axis_Binding : public Input_Binding {
    Input_Axis axis;
    Key key;
    float scale = 1.0f;
    float axis_value = 0;

    Input_Axis_Binding(Input_Axis a, Key k) : axis(a), key(k) {}
};

struct Input_Event {
    Key key;
    float value;
};

struct Input_Manager {
    std::queue<Input_Event> input_event_queue;
    std::vector<Input_Axis_Binding> axis_bindings;
    std::vector<Input_Action_Binding> action_bindings;

    void init();
    void shutdown();

    void register_axis_binding();
    void register_action_binding();

    void process_events();
};

#endif // !ENGINE_INPUT_INPUT_CORE_HPP_INCLUDE