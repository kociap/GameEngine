#include "debug_hotkeys.hpp"

#include "input/input.hpp"

#include "engine.hpp"
#include "shader_manager.hpp"
#include "renderer.hpp"

void Debug_Hotkeys::update() {
    float reload = Input::get_axis_raw("reload_shaders");
    if (reload_old == 0.0f && reload == 1.0f) {
        Engine::get_shader_manager().reload_shaders();
    }
    reload_old = reload;

	float show_shadow_map = Input::get_axis_raw("show_shadow_map");
    if (show_shadow_map == 1.0f && show_shadow_map_old == 0.0f) {
        renderer::Renderer& renderer = Engine::get_renderer();
        renderer.output_shadow_map = !renderer.output_shadow_map;
    }
    show_shadow_map_old = show_shadow_map;
}