#include <engine.hpp>

#include <anton_int.hpp>
#include <anton_stl/utility.hpp>
#include <assets.hpp>
#include <ecs/ecs.hpp>
#include <ecs/entity.hpp>
#include <input/input_core.hpp>
#include <mesh.hpp>
#include <resource_manager.hpp>
#include <time/time_internal.hpp>
#include <utils/filesystem.hpp>
#include <window.hpp>

#include <ecs/jobs_management.hpp>
#include <ecs/system_management.hpp>

#include <builtin_shaders.hpp>
#include <framebuffer.hpp>
#include <glad.hpp>
#include <opengl.hpp>
#include <renderer.hpp>
#include <shader.hpp>

#include <components/camera.hpp>
#include <components/directional_light_component.hpp>
#include <components/point_light_component.hpp>
#include <components/static_mesh_component.hpp>
#include <components/transform.hpp>
#include <debug_hotkeys.hpp>
#include <math/math.hpp>
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

    Framebuffer* Engine::deferred_framebuffer = nullptr;
    Framebuffer* Engine::postprocess_front = nullptr;
    Framebuffer* Engine::postprocess_back = nullptr;

    void Engine::init() {
        time_init();
        main_window = new Window(1280, 720);
        mesh_manager = new Resource_Manager<Mesh>();
        shader_manager = new Resource_Manager<Shader>();
        material_manager = new Resource_Manager<Material>();
        input_manager = new Input::Manager();
        input_manager->load_bindings();
        ecs = new ECS();

        renderer = new rendering::Renderer(main_window->width(), main_window->height());

        Framebuffer::Construct_Info deferred_framebuffer_info;
        deferred_framebuffer_info.width = 1280;
        deferred_framebuffer_info.height = 720;
        deferred_framebuffer_info.depth_buffer.enabled = true;
        deferred_framebuffer_info.depth_buffer.buffer_type = Framebuffer::Buffer_Type::texture;
        deferred_framebuffer_info.color_buffers.resize(2);
        // Normal
        deferred_framebuffer_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgb32f;
        // albedo-specular
        deferred_framebuffer_info.color_buffers[1].internal_format = Framebuffer::Internal_Format::rgba8;
        deferred_framebuffer = new Framebuffer(deferred_framebuffer_info);

        Framebuffer::Construct_Info postprocess_info;
        postprocess_info.width = 1280;
        postprocess_info.height = 720;
        postprocess_info.color_buffers.resize(1);
        postprocess_info.color_buffers[1].internal_format = Framebuffer::Internal_Format::rgba8;
        postprocess_back = new Framebuffer(postprocess_info);
        postprocess_front = new Framebuffer(postprocess_info);

        init_systems();

        load_world();
    }

#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>

    // BS code to output anything on the screen
    void Engine::load_world() {
        auto basic_frag = assets::load_shader_file("basicfrag.frag");
        auto basic_vert = assets::load_shader_file("basicvertex.vert");
        Shader default_shader = create_shader(basic_vert, basic_frag);
        Handle<Shader> default_shader_handle = shader_manager->add(std::move(default_shader));

        auto unlit_vert = assets::load_shader_file("unlit_default.vert");
        auto unlit_frag = assets::load_shader_file("unlit_default.frag");
        Shader unlit_default_shader = create_shader(unlit_vert, unlit_frag);
        /* Handle<Shader> unlit_default_shader_handle = */ shader_manager->add(std::move(unlit_default_shader));

        Mesh container = assets::load_mesh("barrel", 1);
        Material barrel_mat;
        {
            anton_stl::Vector<uint8_t> pixels;
            Texture_Format const format = assets::load_texture_no_mipmaps("barrel_texture", 0, pixels);
            Texture handle;
            void* pix_data = pixels.data();
            rendering::load_textures_generate_mipmaps(format, 1, &pix_data, &handle);
            barrel_mat.diffuse_texture = handle;
            barrel_mat.specular_texture = Texture::default_black;
            barrel_mat.normal_map = Texture::default_normal_map;
        }
        Handle<Material> const material_handle = material_manager->add(std::move(barrel_mat));
        Handle<Mesh> const box_handle = mesh_manager->add(std::move(container));

        Handle<Mesh> const quad_mesh = mesh_manager->add(generate_plane());

        if constexpr (DESERIALIZE) {
            std::filesystem::path serialization_in_path = utils::concat_paths(paths::project_directory(), "ecs.bin");
            std::ifstream file(serialization_in_path, std::ios::binary);
            serialization::Binary_Input_Archive in_archive(file);
            deserialize(in_archive, *ecs);
        } else {
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
        }
    }

    void Engine::terminate() {
        delete renderer;
        renderer = nullptr;
        unload_builtin_shaders();
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

    static void render_frame(Framebuffer* const framebuffer, Framebuffer* const postprocess_back, Matrix4 const view_mat, Matrix4 const inv_view_mat,
                             Matrix4 const projection_mat, Matrix4 const inv_proj_mat, Transform const camera_transform, Vector2 const viewport_size) {
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, viewport_size.x, viewport_size.y);
        bind_framebuffer(framebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        rendering::bind_mesh_vao();
        ECS& ecs = Engine::get_ecs();
        ECS snapshot = ecs.snapshot<Transform, Static_Mesh_Component>();
        rendering::render_scene(snapshot, camera_transform, view_mat, projection_mat);

        // Postprocessing

        glDisable(GL_DEPTH_TEST);
        bind_framebuffer(postprocess_back);
        glBindTextureUnit(0, framebuffer->get_depth_texture());
        glBindTextureUnit(1, framebuffer->get_color_texture(0));
        glBindTextureUnit(2, framebuffer->get_color_texture(1));

        Shader& deferred_shading = get_builtin_shader(Builtin_Shader::deferred_shading);
        deferred_shading.use();
        deferred_shading.set_vec3("camera.position", camera_transform.local_position);
        deferred_shading.set_vec2("viewport_size", viewport_size);
        deferred_shading.set_matrix4("inv_view_mat", inv_view_mat);
        deferred_shading.set_matrix4("inv_proj_mat", inv_proj_mat);
        rendering::render_texture_quad();
    }

    void Engine::loop() {
        main_window->poll_events();
        time_update();
        input_manager->process_events();

        auto camera_mov_view = ecs->view<Camera_Movement, Camera, Transform>();
        for (Entity const entity: camera_mov_view) {
            auto [camera_mov, camera, transform] = camera_mov_view.get<Camera_Movement, Camera, Transform>(entity);
            Camera_Movement::update(camera_mov, camera, transform);
        }

        auto dbg_hotkeys = ecs->view<Debug_Hotkeys>();
        for (Entity const entity: dbg_hotkeys) {
            Debug_Hotkeys::update(dbg_hotkeys.get(entity));
        }

        update_systems();
        execute_jobs();

        // TODO make this rendering code great again (not that it ever was great, but still)
        rendering::update_dynamic_lights();
        auto cameras = ecs->view<Camera, Transform>();
        for (Entity const entity: cameras) {
            auto [camera, transform] = cameras.get<Camera, Transform>(entity);
            if (camera.active) {
                Matrix4 const view_mat = get_camera_view_matrix(transform);
                Matrix4 const inv_view_mat = math::inverse(view_mat);
                Matrix4 const proj_mat = get_camera_projection_matrix(camera, main_window->width(), main_window->height());
                Matrix4 const inv_proj_mat = math::inverse(proj_mat);
                // TODO: Fix shitcode.
                render_frame(deferred_framebuffer, postprocess_back, view_mat, inv_view_mat, proj_mat, inv_proj_mat, transform,
                             Vector2(main_window->width(), main_window->height()));
                anton_stl::swap(postprocess_front, postprocess_back);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glBindTextureUnit(0, postprocess_front->get_color_texture(0));
                Shader& gamma_correction_shader = get_builtin_shader(Builtin_Shader::gamma_correction);
                gamma_correction_shader.use();
                gamma_correction_shader.set_float("gamma", 1 / 2.2f);
                rendering::render_texture_quad();
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
