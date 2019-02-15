#include "engine.hpp"

#include "assets.hpp"
#include "components/component_system.hpp"
#include "debug_macros.hpp"
#include "game_object.hpp"
#include "input/input_core.hpp"
#include "mesh/mesh_manager.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "shader_manager.hpp"
#include "time/time_core.hpp"
#include "utils/path.hpp"
#include "window.hpp"

#include "components/camera.hpp"
#include "components/point_light_component.hpp"
#include "components/static_mesh_component.hpp"
#include "mesh/plane.hpp"
#include "scripts/camera_movement.hpp"

Input::Manager* Engine::input_manager = nullptr;
renderer::Renderer* Engine::renderer = nullptr;
Time_Core* Engine::time_core = nullptr;
Component_System* Engine::component_system = nullptr;
Window* Engine::main_window = nullptr;
Mesh_Manager* Engine::mesh_manager = nullptr;
Shader_Manager* Engine::shader_manager = nullptr;

std::vector<Game_Object> Engine::game_objects{};

void Engine::init(int argc, char** argv) {
    std::filesystem::path executable_path(argv[0]);
    executable_path.remove_filename();
    std::filesystem::path assets_path(utils::concat_paths(executable_path, "assets"));
    std::filesystem::path shaders_path(utils::concat_paths(executable_path, "shaders"));
    Assets::init(executable_path, assets_path, shaders_path);

    main_window = new Window(1000, 800);
    mesh_manager = new Mesh_Manager();
    shader_manager = new Shader_Manager();
    time_core = new Time_Core();
    input_manager = new Input::Manager();
    input_manager->load_bindings();
    component_system = new Component_System();
    renderer = new renderer::Renderer();

    Shader default_shader;
    Assets::load_shader_file_and_attach(default_shader, "basicvertex.vert");
    Assets::load_shader_file_and_attach(default_shader, "basicfrag.frag");
    default_shader.link();
    Handle<Shader> default_shader_handle = shader_manager->add(std::move(default_shader));

    // BS code to output anything on the screen
    game_objects.emplace_back();
    Game_Object& box = game_objects.back();
    Transform& box_t = box.add_component<Transform>();
    Static_Mesh_Component& box_sm = box.add_component<Static_Mesh_Component>();

    std::vector<Mesh> meshes = Assets::load_model("cube.obj");
    Texture container_diffuse;
    container_diffuse.id = Assets::load_texture("container.jpg");
    container_diffuse.type = Texture_Type::diffuse;
    Texture container_specular;
    container_specular.id = Assets::load_texture("container_specular.jpg");
    container_specular.type = Texture_Type::specular;
    meshes[0].textures.pop_back();
    meshes[0].textures.push_back(container_diffuse);
    meshes[0].textures.push_back(container_specular);
    Handle<Mesh> box_handle = mesh_manager->add(std::move(meshes[0]));
    box_sm.set_mesh(box_handle);
    box_sm.set_shader(default_shader_handle);

    game_objects.emplace_back();
    Game_Object& floor = game_objects.back();
    Transform& floor_t = floor.add_component<Transform>();
    Static_Mesh_Component& floor_sm = floor.add_component<Static_Mesh_Component>();
    Texture floor_tex;
    floor_tex.id = Assets::load_texture("wood_floor.png");
    floor_tex.type = Texture_Type::diffuse;
    Plane floor_mesh;
    floor_mesh.textures.push_back(floor_tex);
    Handle<Mesh> floor_handle = mesh_manager->add(std::move(floor_mesh));
    floor_sm.set_mesh(floor_handle);
    floor_sm.set_shader(default_shader_handle);
    floor_t.rotate(Vector3::right, math::radians(-90));
    floor_t.translate({0, -2, 0});

    game_objects.emplace_back();
    Game_Object& lamp = game_objects.back();
    Transform& lamp_t = lamp.add_component<Transform>();
    Point_Light_Component& lamp_pl = lamp.add_component<Point_Light_Component>();
    lamp_t.translate({3, 0.75f, 2});
    lamp_pl.intensity = 5;

    game_objects.emplace_back();
    Game_Object& camera = game_objects.back();
    Transform& camera_t = camera.add_component<Transform>();
    Camera& camera_c = camera.add_component<Camera>();
    Camera_Movement& camera_m = camera.add_component<Camera_Movement>();
    camera_t.translate({0, 0, 10});

	renderer->load_shader_light_properties();
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
    delete shader_manager;
    shader_manager = nullptr;
    delete mesh_manager;
    mesh_manager = nullptr;
    delete main_window;
    main_window = nullptr;
}

void Engine::loop() {
    main_window->poll_events();
    input_manager->process_events();
    time_core->update_time();
    component_system->update_behaviours();
    renderer->render_frame();
    main_window->swap_buffers();
}

bool Engine::should_close() {
    return main_window->should_close();
}

Input::Manager& Engine::get_input_manager() {
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

Mesh_Manager& Engine::get_mesh_manager() {
    return *mesh_manager;
}

Time_Core& Engine::get_time_manager() {
    return *time_core;
}

Shader_Manager& Engine::get_shader_manager() {
    return *shader_manager;
}