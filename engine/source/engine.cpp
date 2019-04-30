#include "engine.hpp"

#include "assets.hpp"
#include "debug_macros.hpp"
#include "ecs/ecs.hpp"
#include "ecs/entity.hpp"
#include "input/input_core.hpp"
#include "mesh/mesh_manager.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "shader_manager.hpp"
#include "time/time_core.hpp"
#include "utils/path.hpp"
#include "window.hpp"

#include "components/camera.hpp"
#include "components/directional_light_component.hpp"
#include "components/point_light_component.hpp"
#include "components/static_mesh_component.hpp"
#include "components/transform.hpp"
#include "debug_hotkeys.hpp"
#include "math/math.hpp"
#include "mesh/cube.hpp"
#include "mesh/plane.hpp"
#include "scripts/camera_movement.hpp"

Input::Manager* Engine::input_manager = nullptr;
renderer::Renderer* Engine::renderer = nullptr;
Time_Core* Engine::time_core = nullptr;
ECS* Engine::ecs = nullptr;
Window* Engine::main_window = nullptr;
Mesh_Manager* Engine::mesh_manager = nullptr;
Shader_Manager* Engine::shader_manager = nullptr;

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
    ecs = new ECS();
    renderer = new renderer::Renderer();

    load_world();

    renderer->load_shader_light_properties();
}

void Engine::load_world() {
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

    std::vector<Mesh> meshes = Assets::load_model("barrel.obj");
    auto& container = meshes[0];
    //Cube container;
    Texture container_diffuse;
    container_diffuse.id = Assets::load_srgb_texture("barrel_texture.jpg", false);
    container_diffuse.type = Texture_Type::diffuse;
    // Texture container_specular;
    // container_specular.id = Assets::load_texture("container_specular.jpg");
    // container_specular.type = Texture_Type::specular;
    Texture container_normal;
    container_normal.id = Assets::load_texture("barrel_normal_map.jpg", false);
    container_normal.type = Texture_Type::normal;
    container.textures.clear();
    container.textures.push_back(container_diffuse);
    // meshes[0].textures.push_back(container_specular);
    container.textures.push_back(container_normal);
    Handle<Mesh> box_handle = mesh_manager->add(std::move(container));

    auto instantiate_box = [&, default_shader_handle, box_handle](Vector3 position, float rotation = 0) {
        Entity box = ecs->create();
        Transform& box_t = ecs->add_component<Transform>(box);
        Static_Mesh_Component& box_sm = ecs->add_component<Static_Mesh_Component>(box);
        box_sm.mesh_handle = box_handle;
        box_sm.shader_handle = default_shader_handle;
        box_t.translate(position);
        box_t.rotate(Vector3::forward, math::radians(rotation));
    };

    instantiate_box({0, 0, 0});
    instantiate_box({-5, 7, 2}, 55.0f);
    instantiate_box({-3, -1, 4});
    instantiate_box({0, -1, 4});

    Texture floor_tex;
    floor_tex.id = Assets::load_srgb_texture("wood_floor.png", false);
    floor_tex.type = Texture_Type::diffuse;
    Plane floor_mesh;
    floor_mesh.textures.push_back(floor_tex);
    Handle<Mesh> floor_handle = mesh_manager->add(std::move(floor_mesh));
    /*for (uint32_t i = 0; i < 121; ++i) {
        Entity floor = Entity::instantiate();
        Transform& floor_t = add_component<Transform>(floor);
        Static_Mesh_Component& floor_sm = add_component<Static_Mesh_Component>(floor);
        floor_sm.mesh_handle = floor_handle;
        floor_sm.shader_handle = default_shader_handle;
        floor_t.rotate(Vector3::right, math::radians(-90));
        floor_t.translate({(static_cast<float>(i % 11) - 5.0f) * 2.0f, -2, (static_cast<float>(i / 11) - 5.0f) * 2.0f});
    }*/

    auto instantiate_point_lamp = [](Vector3 position, Color color, float intensity) {
        Entity lamp = ecs->create();
        Transform& lamp_t = ecs->add_component<Transform>(lamp);
        Point_Light_Component& lamp_pl = ecs->add_component<Point_Light_Component>(lamp);
        //Static_Mesh_Component& lamp_sm = add_component<Static_Mesh_Component>(lamp);
        //auto lamp_cube_handle = mesh_manager->add(Cube());
        //lamp_sm.mesh_handle = lamp_cube_handle;
        //lamp_sm.shader_handle = unlit_default_shader_handle;
        //lamp_t.scale({0.2f, 0.2f, 0.2f});
        lamp_t.translate(position);
        lamp_pl.color = color;
        lamp_pl.intensity = intensity;
    };

    instantiate_point_lamp({3, 1.5f, 2}, {1.0f, 0.5f, 0.0f}, 3);
    instantiate_point_lamp({-6, 1.9f, -2}, {0.3f, 0.75f, 0.25f}, 3);
    instantiate_point_lamp({0, 1.3f, 3}, {0.55f, 0.3f, 1.0f}, 3);
    instantiate_point_lamp({-3, -2.0f, 0}, {0.4f, 0.5f, 0.75f}, 3);
    instantiate_point_lamp({1.5f, 2.5f, 1.5f}, {0.0f, 1.0f, 0.5f}, 3);

    Entity camera = ecs->create();
    Transform& camera_t = ecs->add_component<Transform>(camera);
    Camera& camera_c = ecs->add_component<Camera>(camera);
    Camera_Movement& camera_m = ecs->add_component<Camera_Movement>(camera);
    ecs->add_component<Debug_Hotkeys>(camera);
    camera_t.translate({0, 0, 10});

    Entity directional_light = ecs->create();
    Directional_Light_Component& dl_c = ecs->add_component<Directional_Light_Component>(directional_light);
    dl_c.direction = Vector3(1, -1, -1);
    dl_c.intensity = 0.0f;
}

void Engine::terminate() {
    delete renderer;
    renderer = nullptr;
    delete ecs;
    ecs = nullptr;
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
    time_core->update_time();
    input_manager->process_events();

    auto camera_mov_view = ecs->access<Camera_Movement, Camera, Transform>();
    for (Entity const entity: camera_mov_view) {
        auto& [camera_mov, camera, transform] = camera_mov_view.get<Camera_Movement, Camera, Transform>(entity);
        Camera_Movement::update(camera_mov, camera, transform);
    }

    auto dbg_hotkeys = ecs->access<Debug_Hotkeys>();
    for (Entity const entity: dbg_hotkeys) {
        Debug_Hotkeys::update(dbg_hotkeys.get(entity));
    }

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

ECS& Engine::get_ecs() {
    return *ecs;
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
