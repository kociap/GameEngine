#include "engine.hpp"

#include "assets.hpp"
#include "components/component_system.hpp"
#include "debug_macros.hpp"
#include "game_object.hpp"
#include "input/input_core.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "time/time_core.hpp"
#include "utils/path.hpp"
#include "window.hpp"

Input_Manager* Engine::input_manager = nullptr;
renderer::Renderer* Engine::renderer = nullptr;
Time_Core* Engine::time_core = nullptr;
Component_System* Engine::component_system = nullptr;
Window* Engine::main_window = nullptr;

void Engine::init(int argc, char** argv) {
    std::filesystem::path executable_path(argv[0]);
    executable_path.remove_filename();
    std::filesystem::path assets_path(utils::concat_paths(executable_path, "assets"));
    std::filesystem::path shaders_path(utils::concat_paths(executable_path, "shaders"));
    Assets::init(executable_path, assets_path, shaders_path);

    main_window = new Window(1000, 800);
    time_core = new Time_Core();
    input_manager = new Input_Manager();
    component_system = new Component_System();
    renderer = new renderer::Renderer();
    renderer->load_shaders();
}

void Engine::terminate() {
    delete renderer;
    renderer = nullptr;
    delete component_system;
    component_system = nullptr;
    delete input_manager;
    input_manager = nullptr;
    delete time_core;
    time_core = nullptr;
    delete main_window;
    main_window = nullptr;
}

void Engine::loop() {
    main_window->poll_events();
    input_manager->process_events();
    time_core->update_time();

    update_components();

    renderer->render_frame();

    main_window->swap_buffers();
}

bool Engine::should_close() {
    return main_window->should_close();
}

Input_Manager& Engine::get_input_manager() {
    return *input_manager;
}

Window& Engine::get_window() {
    return *main_window;
}

renderer::Renderer& Engine::get_renderer() {
    return *renderer;
}

Component_System& Engine::get_component_system() {
    return *component_system;
}

void Engine::update_components() {}