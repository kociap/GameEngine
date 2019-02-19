#include "engine.hpp"

#include "assets.hpp"
#include "components/component_system.hpp"
#include "debug_macros.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
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
#include "math/math.hpp"
#include "mesh/cube.hpp"
#include "mesh/plane.hpp"
#include "scripts/camera_movement.hpp"

Input::Manager* Engine::input_manager = nullptr;
renderer::Renderer* Engine::renderer = nullptr;
Time_Core* Engine::time_core = nullptr;
Component_System* Engine::component_system = nullptr;
Window* Engine::main_window = nullptr;
Mesh_Manager* Engine::mesh_manager = nullptr;
Shader_Manager* Engine::shader_manager = nullptr;
Entity_Manager* Engine::entity_manager = nullptr;

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
    entity_manager = new Entity_Manager();
    component_system = new Component_System();
    renderer = new renderer::Renderer();

    Shader default_shader;
    /*Assets::load_shader_file_and_attach(default_shader, "normals.vert");
    Assets::load_shader_file_and_attach(default_shader, "normals.geom");
    Assets::load_shader_file_and_attach(default_shader, "normals.frag");*/
    Assets::load_shader_file_and_attach(default_shader, "basicvertex.vert");
    Assets::load_shader_file_and_attach(default_shader, "basicfrag.frag");
    default_shader.link();

    Handle<Shader> default_shader_handle = shader_manager->add(std::move(default_shader));
    Shader unlit_default_shader;
    Assets::load_shader_file_and_attach(unlit_default_shader, "unlit_default.vert");
    Assets::load_shader_file_and_attach(unlit_default_shader, "unlit_default.frag");
    unlit_default_shader.link();
    Handle<Shader> unlit_default_shader_handle = shader_manager->add(std::move(unlit_default_shader));

    // BS code to output anything on the screen
    Entity box = Entity::instantiate();
    Transform& box_t = add_component<Transform>(box);
    Static_Mesh_Component& box_sm = add_component<Static_Mesh_Component>(box);

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

    Entity floor = Entity::instantiate();
    Transform& floor_t = add_component<Transform>(floor);
    Static_Mesh_Component& floor_sm = add_component<Static_Mesh_Component>(floor);
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

    Entity lamp = Entity::instantiate();
    Transform& lamp_t = add_component<Transform>(lamp);
    Point_Light_Component& lamp_pl = add_component<Point_Light_Component>(lamp);
    Static_Mesh_Component& lamp_sm = add_component<Static_Mesh_Component>(lamp);
    auto lamp_cube_handle = mesh_manager->add(Cube());
    lamp_sm.set_mesh(lamp_cube_handle);
    lamp_sm.set_shader(unlit_default_shader_handle);
    lamp_t.scale({0.2f, 0.2f, 0.2f});
    lamp_t.translate({3, 1.5f, 2});
    lamp_pl.intensity = 10;

    Entity camera = Entity::instantiate();
    Transform& camera_t = add_component<Transform>(camera);
    Camera& camera_c = add_component<Camera>(camera);
    Camera_Movement& camera_m = add_component<Camera_Movement>(camera);
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

Entity_Manager& Engine::get_entity_manager() {
    return *entity_manager;
}