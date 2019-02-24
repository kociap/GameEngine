#include "debug_hotkeys.hpp"

#include "input/input.hpp"

#include "engine.hpp"
#include "shader_manager.hpp"

void Debug_Hotkeys::update() {
    float reload = Input::get_axis_raw("reload_shaders");
    if (reload_old == 0.0f && reload == 1.0f) {
        Engine::get_shader_manager().reload_shaders();
    }

    reload_old = reload;
}