#include <engine.hpp>

#include <anton_int.hpp>
#include <anton_stl/utility.hpp>
#include <assets.hpp>
#include <ecs/ecs.hpp>
#include <ecs/entity.hpp>
#include <input.hpp>
#include <input/input_internal.hpp>
#include <logging.hpp>
#include <mesh.hpp>
#include <paths_internal.hpp>
#include <resource_manager.hpp>
#include <time/time_internal.hpp>
#include <utils/filesystem.hpp>
#include <window.hpp>
#include <windowing_internal.hpp>

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
#include <math/noise.hpp>
#include <paths.hpp>
#include <random.hpp>
#include <scripts/camera_movement.hpp>

#include <build_config.hpp>

// For load_bindings
#include <utils/simple_xml_parser.hpp>

namespace anton_engine {
    static rendering::Renderer* renderer = nullptr;
    static ECS* ecs = nullptr;
    static Window* main_window = nullptr;
    static Resource_Manager<Mesh>* mesh_manager = nullptr;
    static Resource_Manager<Shader>* shader_manager = nullptr;
    static Resource_Manager<Material>* material_manager = nullptr;

    static Framebuffer* deferred_framebuffer = nullptr;
    static Framebuffer* postprocess_front = nullptr;
    static Framebuffer* postprocess_back = nullptr;

    // TODO: Rework.
    static void load_input_bindings() {
        // TODO uses engine exe dir
        std::filesystem::path const bindings_file_path(utils::concat_paths(paths::executable_directory(), "input_bindings.config"));
        std::string const config_file = assets::read_file_raw_string(bindings_file_path);
        {
            auto find_property = [](auto& properties, auto predicate) -> anton_stl::Vector<utils::xml::Tag_Property>::iterator {
                auto end = properties.end();
                for (auto iter = properties.begin(); iter != end; ++iter) {
                    if (predicate(*iter)) {
                        return iter;
                    }
                }
                return end;
            };

            anton_stl::Vector<utils::xml::Tag> tags(utils::xml::parse(config_file));
            for (utils::xml::Tag& tag: tags) {
                if (tag.name != "axis" && tag.name != "action") {
                    ANTON_LOG_INFO("Unknown tag, skipping...");
                    continue;
                }

                auto axis_prop = find_property(tag.properties, [](auto& property) { return property.name == "axis"; });
                auto action_prop = find_property(tag.properties, [](auto& property) { return property.name == "action"; });
                auto key_prop = find_property(tag.properties, [](auto& property) { return property.name == "key"; });
                auto accumulation_speed_prop = find_property(tag.properties, [](auto& property) { return property.name == "scale"; });
                auto sensitivity_prop = find_property(tag.properties, [](auto& property) { return property.name == "sensitivity"; });

                if (axis_prop == tag.properties.end() && action_prop == tag.properties.end()) {
                    ANTON_LOG_INFO("Missing action/axis property, skipping...");
                    continue;
                }
                if (key_prop == tag.properties.end()) {
                    ANTON_LOG_INFO("Missing key property, skipping...");
                    continue;
                }

                if (axis_prop != tag.properties.end()) {
                    if (sensitivity_prop == tag.properties.end()) {
                        ANTON_LOG_INFO("Missing sensitivity property, skipping...");
                        continue;
                    }
                    if (accumulation_speed_prop == tag.properties.end()) {
                        ANTON_LOG_INFO("Missing scale property, skipping...");
                        continue;
                    }
                    input::add_axis(axis_prop->value.data(), key_from_string(key_prop->value), std::stof(sensitivity_prop->value),
                                    std::stof(accumulation_speed_prop->value), false);
                } else {
                    input::add_action(action_prop->value.data(), key_from_string(key_prop->value));
                }
            }
        }
    }

    // TODO: Forward decl. Remove.
    static void load_world();

    static void init() {
        init_time();
        init_windowing();
        enable_vsync(true);
        main_window = create_window(1280, 720, true);
        make_context_current(main_window);
        opengl::load();
        rendering::setup_rendering();
        load_builtin_shaders();

        mesh_manager = new Resource_Manager<Mesh>();
        shader_manager = new Resource_Manager<Shader>();
        material_manager = new Resource_Manager<Material>();
        load_input_bindings();
        ecs = new ECS();

        Vector2 const window_dims = get_window_size(main_window);
        renderer = new rendering::Renderer(window_dims.x, window_dims.y);

        Framebuffer::Construct_Info deferred_framebuffer_info;
        deferred_framebuffer_info.width = window_dims.x;
        deferred_framebuffer_info.height = window_dims.y;
        deferred_framebuffer_info.depth_buffer.enabled = true;
        deferred_framebuffer_info.depth_buffer.buffer_type = Framebuffer::Buffer_Type::texture;
        deferred_framebuffer_info.color_buffers.resize(2);
        // Normal
        deferred_framebuffer_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgb32f;
        // albedo-specular
        deferred_framebuffer_info.color_buffers[1].internal_format = Framebuffer::Internal_Format::rgba8;
        deferred_framebuffer = new Framebuffer(deferred_framebuffer_info);

        Framebuffer::Construct_Info postprocess_info;
        postprocess_info.width = window_dims.x;
        postprocess_info.height = window_dims.y;
        postprocess_info.color_buffers.resize(1);
        postprocess_info.color_buffers[1].internal_format = Framebuffer::Internal_Format::rgba8;
        postprocess_back = new Framebuffer(postprocess_info);
        postprocess_front = new Framebuffer(postprocess_info);

        init_systems();

        load_world();

        start_systems();
    }

#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>

    // BS code to output anything on the screen
    static void load_world() {
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
            // auto create_noise_texture = [](anton_stl::Vector<uint8_t>& pixels) {
            //     i32 const perm_table[] = {
            //         11,  3,   299, 83,  42,  81,  213, 25,  98,  279, 292, 43,  22,  247, 243, 145, 245, 240, 96,  17,  26,  152, 244, 32,  62,  24,  119, 186,
            //         274, 188, 163, 39,  175, 90,  156, 79,  278, 237, 157, 13,  258, 7,   225, 131, 252, 94,  176, 52,  44,  136, 283, 0,   168, 208, 167, 95,
            //         197, 255, 259, 2,   248, 86,  271, 92,  64,  142, 234, 179, 15,  211, 132, 189, 20,  124, 35,  183, 146, 199, 294, 241, 264, 233, 57,  121,
            //         220, 105, 184, 260, 99,  205, 185, 36,  174, 56,  246, 269, 88,  215, 198, 204, 238, 149, 228, 63,  139, 137, 242, 84,  19,  221, 129, 66,
            //         103, 253, 37,  102, 256, 71,  77,  268, 135, 254, 158, 277, 126, 10,  227, 45,  100, 68,  38,  284, 14,  166, 218, 162, 216, 58,  122, 108,
            //         267, 69,  200, 75,  140, 117, 111, 222, 193, 55,  224, 266, 251, 257, 116, 143, 235, 114, 287, 70,  61,  209, 87,  91,  51,  276, 50,  177,
            //         33,  285, 291, 78,  239, 159, 262, 296, 273, 101, 125, 160, 210, 8,   4,   29,  281, 250, 232, 93,  59,  236, 54,  173, 144, 249, 169, 82,
            //         23,  60,  161, 9,   231, 74,  182, 223, 27,  134, 138, 202, 31,  155, 172, 21,  280, 289, 47,  97,  297, 203, 154, 164, 118, 151, 226, 113,
            //         123, 207, 120, 16,  298, 48,  217, 165, 12,  288, 130, 282, 187, 40,  229, 270, 110, 72,  171, 41,  230, 65,  67,  196, 28,  148, 112, 104,
            //         192, 195, 1,   109, 106, 212, 219, 18,  293, 80,  73,  133, 290, 272, 261, 180, 141, 190, 286, 194, 53,  214, 147, 76,  206, 34,  191, 300,
            //         127, 49,  170, 275, 6,   181, 153, 265, 178, 107, 5,   263, 150, 128, 115, 85,  89,  295, 201, 30,  46};

            //     seed_default_random_engine(534432432);
            //     Vector2 gradients[24] = {};
            //     for (i32 i = 0; i < 24; ++i) {
            //         i64 random = random_i64(0, 1440);
            //         f32 angle = f32(random) * 0.25f;
            //         gradients[i].x = cos(math::radians(angle));
            //         gradients[i].y = sin(math::radians(angle));
            //     }

            //     for (i32 y = 0; y < 2048 * 2; ++y) {
            //         for (i32 x = 0; x < 2048 * 2; ++x) {
            //             Vector2 pos(x, y);
            //             pos /= 500.0f;
            //             f32 noise_val = math::perlin_noise(pos, gradients, perm_table);
            //             f32 scaled = noise_val * 127.5f + 127.5f;
            //             pixels.push_back(scaled);
            //         }
            //     }

            //     Texture_Format format;
            //     format.width = 2048 * 2;
            //     format.height = 2048 * 2;
            //     format.sized_internal_format = GL_R8;
            //     format.pixel_format = GL_RED;
            //     format.pixel_type = GL_UNSIGNED_BYTE;
            //     format.mip_levels = 1;
            //     format.filter = GL_NEAREST_MIPMAP_NEAREST;
            //     format.swizzle_mask[0] = GL_RED;
            //     format.swizzle_mask[1] = GL_RED;
            //     format.swizzle_mask[2] = GL_RED;
            //     format.swizzle_mask[3] = GL_ONE;
            //     return format;
            // };

            // Texture_Format const format = create_noise_texture(pixels);
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
            // std::filesystem::path serialization_in_path = utils::concat_paths(paths::project_directory(), "ecs.bin");
            // std::ifstream file(serialization_in_path, std::ios::binary);
            // serialization::Binary_Input_Archive in_archive(file);
            // deserialize(in_archive, *ecs);
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

    static void terminate() {
        delete renderer;
        renderer = nullptr;
        unload_builtin_shaders();
        delete ecs;
        ecs = nullptr;
        delete material_manager;
        material_manager = nullptr;
        delete shader_manager;
        shader_manager = nullptr;
        delete mesh_manager;
        mesh_manager = nullptr;
        destroy_window(main_window);
        main_window = nullptr;
        terminate_windowing();
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

    static void loop() {
        poll_window_events();
        update_time();
        input::process_events();

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
                Vector2 const window_dims = get_window_size(main_window);
                Matrix4 const view_mat = get_camera_view_matrix(transform);
                Matrix4 const inv_view_mat = math::inverse(view_mat);
                Matrix4 const proj_mat = get_camera_projection_matrix(camera, window_dims.x, window_dims.y);
                Matrix4 const inv_proj_mat = math::inverse(proj_mat);
                // TODO: Fix shitcode.
                render_frame(deferred_framebuffer, postprocess_back, view_mat, inv_view_mat, proj_mat, inv_proj_mat, transform,
                             Vector2(window_dims.x, window_dims.y));
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

        swap_buffers(main_window);
    }

    int engine_main(int argc, char** argv) {
        std::filesystem::path exe_directory(argv[0], std::filesystem::path::generic_format);
        paths::set_executable_name(exe_directory.filename());
        exe_directory.remove_filename();
        paths::set_executable_directory(exe_directory);

        init();
        while (!should_close(main_window)) {
            loop();
        }
        terminate();

        return 0;
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
