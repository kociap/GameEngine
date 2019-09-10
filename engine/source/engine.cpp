#include <engine.hpp>

#include <assets.hpp>
#include <ecs/ecs.hpp>
#include <ecs/entity.hpp>
#include <input/input_core.hpp>
#include <mesh/mesh.hpp>
#include <renderer.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>
#include <time/time_core.hpp>
#include <utils/filesystem.hpp>
#include <window.hpp>

#include <components/camera.hpp>
#include <components/directional_light_component.hpp>
#include <components/point_light_component.hpp>
#include <components/static_mesh_component.hpp>
#include <components/transform.hpp>
#include <debug_hotkeys.hpp>
#include <math/math.hpp>
#include <mesh/cube.hpp>
#include <mesh/plane.hpp>
#include <paths.hpp>
#include <scripts/camera_movement.hpp>

#include <build_config.hpp>

namespace anton_engine {
    Input::Manager* Engine::input_manager = nullptr;
    rendering::Renderer* Engine::renderer = nullptr;
    ECS* Engine::ecs = nullptr;
    Window* Engine::main_window = nullptr;
    Resource_Manager<Mesh>* Engine::mesh_manager = nullptr;
    Resource_Manager<Shader>* Engine::shader_manager = nullptr;
    Resource_Manager<Material>* Engine::material_manager = nullptr;

    void Engine::init() {
        time_core::init();
        main_window = new Window(1280, 720);
        mesh_manager = new Resource_Manager<Mesh>();
        shader_manager = new Resource_Manager<Shader>();
        material_manager = new Resource_Manager<Material>();
        input_manager = new Input::Manager();
        input_manager->load_bindings();
        ecs = new ECS();

        renderer = new rendering::Renderer(main_window->width(), main_window->height());
        renderer->load_shader_light_properties();
        load_world();
    }

#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>

    void Engine::load_world() {
        // #define RENDER_CUBES

        /*assets::load_shader_file_and_attach(default_shader, "normals.vert");
        assets::load_shader_file_and_attach(default_shader, "normals.geom");
        assets::load_shader_file_and_attach(default_shader, "normals.frag");*/
#ifdef RENDER_CUBES
        auto basic_frag = assets::load_shader_file("basicfrag.2.frag");
#else
        auto basic_frag = assets::load_shader_file("basicfrag.frag");
#endif
        auto basic_vert = assets::load_shader_file("basicvertex.vert");
        Shader default_shader = create_shader(basic_vert, basic_frag);
        Handle<Shader> default_shader_handle = shader_manager->add(std::move(default_shader));

        auto unlit_vert = assets::load_shader_file("unlit_default.vert");
        auto unlit_frag = assets::load_shader_file("unlit_default.frag");
        Shader unlit_default_shader = create_shader(unlit_vert, unlit_frag);
        /* Handle<Shader> unlit_default_shader_handle = */ shader_manager->add(std::move(unlit_default_shader));

        // BS code to output anything on the screen
#ifdef RENDER_CUBES
        anton_stl::Vector<Mesh> meshes = assets::load_model("cube.obj");
        auto& container = meshes[0];
        Handle<Material> material_handle = material_manager->add(Material());
#else
        anton_stl::Vector<Mesh> meshes = assets::load_model("barrel.obj");
        auto& container = meshes[0];
        Material barrel_mat;
        barrel_mat.diffuse_texture.handle = assets::load_texture("barrel_texture", 0);
        barrel_mat.normal_map.handle = assets::load_texture("barrel_normal_map", 0);
        Handle<Material> material_handle = material_manager->add(std::move(barrel_mat));
#endif
        Handle<Mesh> box_handle = mesh_manager->add(std::move(container));
        Handle<Mesh> quad_mesh = mesh_manager->add(Plane());

#if DESERIALIZE
        std::filesystem::path serialization_in_path = utils::concat_paths(paths::project_directory(), "ecs.bin");
        std::ifstream file(serialization_in_path, std::ios::binary);
        serialization::Binary_Input_Archive in_archive(file);
        serialization::deserialize(in_archive, *ecs);
#else
        auto instantiate_box = [default_shader_handle, box_handle, material_handle](Vector3 position, float rotation = 0) {
            Entity box = ecs->create();
            Transform& box_t = ecs->add_component<Transform>(box);
            Static_Mesh_Component& box_sm = ecs->add_component<Static_Mesh_Component>(box);
            box_sm.mesh_handle = box_handle;
            box_sm.shader_handle = default_shader_handle;
            box_sm.material_handle = material_handle;
            box_t.translate(position);
            box_t.rotate(Vector3::forward, math::radians(rotation));
        };

        instantiate_box({0, 0, -1});
        instantiate_box({-5, 7, 2}, 55.0f);
        instantiate_box({-3, -1, 4});
        instantiate_box({0, -1, 4});

        Entity quad = ecs->create();
        Static_Mesh_Component& quad_sm = ecs->add_component<Static_Mesh_Component>(quad);
        quad_sm.material_handle = material_handle;
        quad_sm.mesh_handle = quad_mesh;
        quad_sm.shader_handle = default_shader_handle;
        ecs->add_component<Transform>(quad);

        // Plane floor_mesh;
        // floor_mesh.material.diffuse_texture.handle = assets::load_srgb_texture("wood_floor.png", false);
        // Handle<Mesh> floor_handle = mesh_manager->add(std::move(floor_mesh));
        // for (uint32_t i = 0; i < 121; ++i) {
        //     Entity floor = Entity::instantiate();
        //     Transform& floor_t = add_component<Transform>(floor);
        //     Static_Mesh_Component& floor_sm = add_component<Static_Mesh_Component>(floor);
        //     floor_sm.mesh_handle = floor_handle;
        //     floor_sm.shader_handle = default_shader_handle;
        //     floor_t.rotate(Vector3::right, math::radians(-90));
        //     floor_t.translate({(static_cast<float>(i % 11) - 5.0f) * 2.0f, -2, (static_cast<float>(i / 11) - 5.0f) * 2.0f});
        // }

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

        /*instantiate_point_lamp({3, 1.5f, 2}, {1.0f, 0.5f, 0.0f}, 3);
        instantiate_point_lamp({-6, 1.9f, -2}, {0.3f, 0.75f, 0.25f}, 3);
        instantiate_point_lamp({0, 1.3f, 3}, {0.55f, 0.3f, 1.0f}, 3);
        instantiate_point_lamp({-3, -2.0f, 0}, {0.4f, 0.5f, 0.75f}, 3);
        instantiate_point_lamp({1.5f, 2.5f, 1.5f}, {0.0f, 1.0f, 0.5f}, 3);*/

        instantiate_point_lamp({-3, 3, -3}, {1, 1, 1}, 3);
        instantiate_point_lamp({-3, 3, 3}, {1, 1, 1}, 3);
        instantiate_point_lamp({3, 3, -3}, {1, 1, 1}, 3);
        instantiate_point_lamp({3, 3, 3}, {1, 1, 1}, 3);
        instantiate_point_lamp({-3, -3, -3}, {1, 1, 1}, 3);
        instantiate_point_lamp({-3, -3, 3}, {1, 1, 1}, 3);
        instantiate_point_lamp({3, -3, -3}, {1, 1, 1}, 3);
        instantiate_point_lamp({3, -3, 3}, {1, 1, 1}, 3);

        Entity camera = ecs->create();
        Transform& camera_t = ecs->add_component<Transform>(camera);
        Camera& camera_c = ecs->add_component<Camera>(camera);
        camera_c.near_plane = 0.05f;
        ecs->add_component<Camera_Movement>(camera);
        ecs->add_component<Debug_Hotkeys>(camera);
        camera_t.translate({0, 0, 0});

        Entity directional_light = ecs->create();
        Directional_Light_Component& dl_c = ecs->add_component<Directional_Light_Component>(directional_light);
        dl_c.direction = Vector3(1, -1, -1);
        dl_c.intensity = 0.0f;
#endif
    }

    void Engine::terminate() {
        delete renderer;
        renderer = nullptr;
        delete ecs;
        ecs = nullptr;
        delete input_manager;
        input_manager = nullptr;
        delete material_manager;
        material_manager = nullptr;
        delete shader_manager;
        shader_manager = nullptr;
        delete mesh_manager;
        mesh_manager = nullptr;
        delete main_window;
        main_window = nullptr;
    }

    void Engine::loop() {
        main_window->poll_events();
        time_core::update_time();
        input_manager->process_events();

        auto camera_mov_view = ecs->access<Camera_Movement, Camera, Transform>();
        for (Entity const entity: camera_mov_view) {
            auto [camera_mov, camera, transform] = camera_mov_view.get<Camera_Movement, Camera, Transform>(entity);
            Camera_Movement::update(camera_mov, camera, transform);
        }

        auto dbg_hotkeys = ecs->access<Debug_Hotkeys>();
        for (Entity const entity: dbg_hotkeys) {
            Debug_Hotkeys::update(dbg_hotkeys.get(entity));
        }

        // TODO make this rendering code great again (not that it ever was great, but still)
        auto rendering = ecs->access<Camera, Transform>();
        for (Entity const entity: rendering) {
            auto [camera, transform] = rendering.get<Camera, Transform>(entity);
            if (camera.active) {
                Matrix4 const view_mat = get_camera_view_matrix(transform);
                Matrix4 const proj_mat = get_camera_projection_matrix(camera, main_window->width(), main_window->height());
                renderer->render_frame(view_mat, proj_mat, transform, main_window->width(), main_window->height());
                break;
            }
        }

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

    rendering::Renderer& Engine::get_renderer() {
        return *renderer;
    }

    ECS& Engine::get_ecs() {
        return *ecs;
    }

    Resource_Manager<Mesh>& Engine::get_mesh_manager() {
        return *mesh_manager;
    }

    Resource_Manager<Material>& Engine::get_material_manager() {
        return *material_manager;
    }

    Resource_Manager<Shader>& Engine::get_shader_manager() {
        return *shader_manager;
    }
} // namespace anton_engine
