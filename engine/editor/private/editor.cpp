#include <editor.hpp>

#include <build_config.hpp>
#include <shaders/builtin_shaders.hpp>
#include <scripts/camera_movement.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/entity_name.hpp>
#include <engine/components/transform.hpp>
#include <scripts/debug_hotkeys.hpp>
#include <core/diagnostic_macros.hpp>
#include <engine/ecs/ecs.hpp>
#include <editor_window.hpp>
#include <engine/input.hpp>
#include <engine/input/input_internal.hpp>
#include <engine/material.hpp>
#include <engine/mesh.hpp>
#include <rendering/opengl.hpp>
#include <core/paths.hpp>
#include <core/paths_internal.hpp>
#include <rendering/renderer.hpp>
#include <engine/resource_manager.hpp>
#include <shaders/shader.hpp>
#include <engine/time_internal.hpp>
#include <core/utils/filesystem.hpp>
#include <windowing/window.hpp>

#include <rendering/builtin_editor_shaders.hpp>
#include <rendering/glad.hpp>
#include <imgui/imgui.hpp>
#include <rendering/imgui_rendering.hpp>

// TODO: Remove
#include <core/logging.hpp>
#include <core/serialization/archives/binary.hpp>
#include <core/serialization/serialization.hpp>
#include <engine/time.hpp>
#include <stdio.h>

#include <engine/time.hpp>

namespace anton_engine {
    static bool _quit = false;

    // TODO: Temporarily
    static windowing::Window* main_window = nullptr;
    static windowing::OpenGL_Context* gl_context;
    static Resource_Manager<Mesh>* mesh_manager = nullptr;
    static Resource_Manager<Shader>* shader_manager = nullptr;
    static Resource_Manager<Material>* material_manager = nullptr;
    static ECS* ecs = nullptr;
    static imgui::Context* imgui_context = nullptr;
    static rendering::Font_Face* comic_sans_face = nullptr;
    static rendering::Font_Face* french_script_regular_face = nullptr;

    void quit() {
        _quit = true;
    }

    static void loop() {
        windowing::poll_events();
        update_time();

        // printf("delta time: %llf\n", get_delta_time());

        // ANTON_LOG_INFO(atl::to_string(get_delta_time()));
        {
            // printf("%d %d %d %d\n", mimas_get_key(MIMAS_KEY_A), mimas_get_key(MIMAS_KEY_NUMPAD_ENTER), mimas_get_key(MIMAS_KEY_SLASH), mimas_get_key(MIMAS_KEY_NUMPAD_DIVIDE));

            imgui::Context& ctx = *imgui_context;
            imgui::Input_State imgui_input = imgui::get_input_state(ctx);
            imgui_input.cursor_position = windowing::get_cursor_pos();
            imgui_input.left_mouse_button = windowing::get_key(Key::left_mouse_button);
            imgui::set_input_state(ctx, imgui_input);

            imgui::begin_frame(ctx);
            {
                imgui::begin_window(ctx, "main_window");
                imgui::Button_Style button_style;
                button_style.background_color = {0.1f, 0.1f, 0.1f};
                button_style.border_color = {1.0f, 1.0f, 1.0f};
                button_style.border = {2.0f, 2.0f, 2.0f, 2.0f};
                button_style.padding = {5.0f, 10.0f, 5.0f, 10.0f};
                button_style.font.face = comic_sans_face;
                button_style.font.size = 12;
                button_style.font.v_dpi = 96;
                button_style.font.h_dpi = 96;
                imgui::Button_Style hot_style = button_style;
                hot_style.background_color = {0.2f, 0.5f, 0.8f};
                hot_style.border_color = {1.0f, 0.0f, 0.0f};
                hot_style.font.face = french_script_regular_face;
                hot_style.font.size = 16;
                imgui::Button_State state = imgui::button(ctx, "Confirm", button_style, hot_style, button_style);
                switch(state) {
                    case imgui::Button_State::hot: {
                        imgui::button(ctx, "DoctorExpress Deluxe", button_style, hot_style, button_style);
                    } break;

                    case imgui::Button_State::clicked: {
                        imgui::button(ctx, "Pengu's Button", button_style, hot_style, button_style);
                    } break;
                }

                imgui::Style main_style = imgui::get_style(ctx);
                main_style.background_color = {0.953f, 0.322f, 0.125f};
                // main_style.background_color = {0.7f, 0.2f, 0.2f};
                // if (imgui::is_window_hot(ctx)) {
                //     main_style.background_color = {0.6f, 0.4f, 0.4f};
                //     // ANTON_LOG_INFO("main_window hot");
                // }

                // if (imgui::is_window_active(ctx)) {
                //     main_style.background_color = {0.6f, 0.6f, 0.6f};
                //     // ANTON_LOG_INFO("main_window active");
                // }
                imgui::set_style(ctx, main_style);
                imgui::end_window(ctx);
            }

            imgui::begin_window(ctx, "secondary_window");
            imgui::button(ctx, "Another Window");
            imgui::Style secondary_style = imgui::get_style(ctx);
            secondary_style.background_color = {0.51f, 0.74f, 0.4f};
            // secondary_style.background_color = {112.0f / 255.0f, 0.0f, 1.0f};
            // if (imgui::is_window_hot(ctx)) {
            //     secondary_style.background_color = {146.0f / 255.0f, 56.0f / 255.0f, 1.0f};
            //     // ANTON_LOG_INFO("secondary_window hot");
            // }

            // if (imgui::is_window_active(ctx)) {
            //     secondary_style.background_color = {175.0f / 255.0f, 110.0f / 255.0f, 1.0f};
            //     // ANTON_LOG_INFO("secondary_window active");
            // }
            imgui::set_style(ctx, secondary_style);
            imgui::end_window(ctx);

            imgui::begin_window(ctx, "third_window");
            imgui::Style third_style = imgui::get_style(ctx);
            third_style.background_color = {0.0f, 0.659f, 0.941f};
            // third_style.background_color = {96.0f / 255.0f, 214.0f / 255.0f, 0.0f};
            // if (imgui::is_window_hot(ctx)) {
            //     third_style.background_color = {115.0f / 255.0f, 1.0f, 0.0f};
            // }

            // if (imgui::is_window_active(ctx)) {
            //     third_style.background_color = {144.0f / 255.0f, 1.0f, 77.0f / 255.0f};
            // }
            imgui::set_style(ctx, third_style);
            imgui::end_window(ctx);

            {
                imgui::begin_window(ctx, "window4");
                imgui::Style style = imgui::get_style(ctx);
                style.background_color = {1.0f, 0.733f, 0.008f};
                // style.background_color = {0.2f, 0.2f, 0.2f};
                // if (imgui::is_window_hot(ctx)) {
                //     style.background_color = {0.4f, 0.4f, 0.4f};
                // }
                imgui::set_style(ctx, style);
                imgui::end_window(ctx);
            }
            {
                imgui::begin_window(ctx, "window5");
                imgui::Style style = imgui::get_style(ctx);
                style.background_color = {0.7f, 0.933f, 0.008f};
                // style.background_color = {0.2f, 0.2f, 0.2f};
                // if (imgui::is_window_hot(ctx)) {
                //     style.background_color = {0.4f, 0.4f, 0.4f};
                // }
                imgui::set_style(ctx, style);
                imgui::end_window(ctx);
            }
            {
                imgui::begin_window(ctx, "window6");
                imgui::Style style = imgui::get_style(ctx);
                style.background_color = {1.0f, 0.533f, 0.308f};
                // style.background_color = {0.2f, 0.2f, 0.2f};
                // if (imgui::is_window_hot(ctx)) {
                //     style.background_color = {0.4f, 0.4f, 0.4f};
                // }
                imgui::set_style(ctx, style);
                imgui::end_window(ctx);
            }

            imgui::end_frame(ctx);

            atl::Slice<imgui::Vertex const> vertices = imgui::get_vertex_data(ctx);
            atl::Slice<u32 const> indices = imgui::get_index_data(ctx);
            imgui::Draw_Elements_Command cmd = imgui::write_geometry(vertices, indices);
            // TODO: Add textures.
            cmd.instance_count = 1;
            cmd.base_instance = 0;
            Shader& imgui_shader = get_builtin_shader(Builtin_Editor_Shader::imgui);
            atl::Slice<imgui::Viewport* const> const viewports = imgui::get_viewports(ctx);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            imgui_shader.use();
            imgui::bind_buffers();
            for (imgui::Viewport* viewport: viewports) {
                windowing::Window* native_window = imgui::get_viewport_native_window(ctx, *viewport);
                windowing::make_context_current(gl_context, native_window);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                Vector2 const window_size = windowing::get_window_size(native_window);
                Vector2 const window_pos = windowing::get_window_pos(native_window);
                glViewport(0, 0, window_size.x, window_size.y);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Matrix4 const imgui_projection =
                    math::transform::orthographic(window_pos.x, window_pos.x + window_size.x, window_pos.y + window_size.y, window_pos.y, 1.0f, -1.0f);
                imgui_shader.set_matrix4("proj_mat", imgui_projection);

                atl::Slice<imgui::Draw_Command const> draw_commands = imgui::get_viewport_draw_commands(ctx, *viewport);
                imgui_shader.set_int("texture_bound", 0);
                u32 last_bound_texture = 0;
                for (imgui::Draw_Command draw_command: draw_commands) {
                    if(last_bound_texture != draw_command.texture) {
                        imgui::commit_draw();
                        glBindTextureUnit(0, draw_command.texture);
                        imgui_shader.set_int("texture_bound", draw_command.texture != 0);
                        last_bound_texture = draw_command.texture;
                    }
                    imgui::Draw_Elements_Command viewport_cmd = cmd;
                    viewport_cmd.count = draw_command.element_count;
                    viewport_cmd.base_vertex += draw_command.vertex_offset;
                    viewport_cmd.first_index += draw_command.index_offset;
                    imgui::add_draw_command(viewport_cmd);
                }
                imgui::commit_draw();
                windowing::swap_buffers(native_window);
            }
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }

        ecs->remove_requested_entities();
    }

    // TODO: Forward decl of load_world. Remove (eventually)
    static void load_world();

    static void cursor_pos_callback(windowing::Window* const window, f32 const x, f32 const y, void* const data) {
        // printf("cursor move: %f %f\n", x, y);
    }

    static void mouse_button_callback(windowing::Window* const window, Key const button, i32 const action, void* const data) {
        // static bool is_window_fullscreen = true;
        // if(button == Key::left_mouse_button && action == 1) {
        //     if(is_window_fullscreen) {
        //         windowing::fullscreen_window(window, nullptr);
        //         windowing::set_size(window, {1280.0f, 720.0f});
        //     } else {
        //         windowing::Display* display = windowing::get_primary_display();
        //         windowing::fullscreen_window(window, display);
        //     }

        //     is_window_fullscreen = !is_window_fullscreen;
        // }
    }

    static void init() {
        init_time();
        windowing::init();
        windowing::enable_vsync(true);
        main_window = windowing::create_window(1280, 720, true);
        windowing::Display* primary_display = windowing::get_primary_display();
        // windowing::fullscreen_window(main_window, primary_display);
        windowing::set_mouse_button_callback(main_window, mouse_button_callback, nullptr);
        windowing::set_cursor_pos_callback(main_window, cursor_pos_callback, nullptr);
        gl_context = windowing::create_context(4, 5, windowing::OpenGL_Profile::core);
        windowing::make_context_current(gl_context, main_window);
        opengl::load();
        rendering::setup_rendering();
        rendering::init_font_rendering();
        atl::String comic_path = paths::assets_directory().generic_string().data();
        comic_path.append(u8"/fonts/comic.ttf");
        comic_sans_face = rendering::load_face_from_file(comic_path, 0);
        atl::String french_script_regular = paths::assets_directory().generic_string().data();
        french_script_regular.append(u8"/fonts/french_script_regular.ttf");
        french_script_regular_face = rendering::load_face_from_file(french_script_regular, 0);
        load_builtin_shaders();

        mesh_manager = new Resource_Manager<Mesh>();
        shader_manager = new Resource_Manager<Shader>();
        material_manager = new Resource_Manager<Material>();
        ecs = new ECS();

        {
            imgui::setup_rendering();
            imgui::Font_Style font_style;
            font_style.face = comic_sans_face;
            font_style.size = 12;
            font_style.h_dpi = 96;
            font_style.v_dpi = 96;
            imgui_context = imgui::create_context(font_style);
            imgui::set_main_viewport_native_window(*imgui_context, main_window);
        }

        imgui::Context& ctx = *imgui_context;
        imgui::begin_frame(ctx);
        imgui::begin_window(ctx, "main_window");
        imgui::set_window_size(ctx, {200, 200});
        imgui::set_window_pos(ctx, {150, 150});
        imgui::end_window(ctx);
        imgui::begin_window(ctx, "secondary_window");
        imgui::set_window_size(ctx, {200, 200});
        imgui::set_window_pos(ctx, {370, 150});
        imgui::end_window(ctx);
        imgui::begin_window(ctx, "third_window");
        imgui::set_window_size(ctx, {200, 200});
        imgui::set_window_pos(ctx, {150, 400});
        imgui::end_window(ctx);
        imgui::begin_window(ctx, "window4");
        imgui::set_window_size(ctx, {200, 200});
        imgui::set_window_pos(ctx, {370, 400});
        imgui::end_window(ctx);
        imgui::begin_window(ctx, "window5");
        imgui::set_window_size(ctx, {200, 200});
        imgui::set_window_pos(ctx, {150, 650});
        imgui::end_window(ctx);
        imgui::begin_window(ctx, "window6");
        imgui::set_window_size(ctx, {200, 200});
        imgui::set_window_pos(ctx, {370, 650});
        imgui::end_window(ctx);
        imgui::end_frame(ctx);

        load_world();
    }

    static void terminate() {
#if SERIALIZE_ON_QUIT
        std::filesystem::path serialization_out_path = utils::concat_paths(paths::project_directory(), "ecs.bin");
        std::ofstream file(serialization_out_path, std::ios::binary | std::ios::trunc);
        serialization::Binary_Output_Archive out_archive(file);
        serialize(out_archive, Engine::get_ecs());
#endif
        rendering::terminate_font_rendering();
        windowing::terminate();
    }

    int editor_main(int argc, char** argv) {
        // Required arguments: path to executable and path to the project file
        // if (argc < 2) {
        //     throw std::runtime_error("Missing path to project file");
        // }

        // if(!std::filesystem::exists(argv[1])) {
        //     throw std::runtime_error("Specified project file does not exist");
        // }

        std::filesystem::path exe_directory(argv[0], std::filesystem::path::generic_format);
        paths::set_executable_name(exe_directory.filename());
        exe_directory.remove_filename();
        paths::set_executable_directory(exe_directory);
        //std::filesystem::path project_directory(argv[1], std::filesystem::path::generic_format);
        std::filesystem::path project_directory("C:/Users/An0num0us/Documents/GameEngine_Game/GameEngine_Game.geproject",
                                                std::filesystem::path::generic_format);
        project_directory.remove_filename();
        paths::set_project_directory(project_directory);

        // TODO Validate project file

        init();
        while (!_quit) {
            loop();
        }
        terminate();
        return 0;
    }

    Resource_Manager<Mesh>& Editor::get_mesh_manager() {
        return *mesh_manager;
    }

    Resource_Manager<Shader>& Editor::get_shader_manager() {
        return *shader_manager;
    }

    Resource_Manager<Material>& Editor::get_material_manager() {
        return *material_manager;
    }

    ECS& Editor::get_ecs() {
        return *ecs;
    }

} // namespace anton_engine

#include <content_browser/asset_guid.hpp>
#include <content_browser/asset_importing.hpp>
#include <engine/assets.hpp>
#include <engine/components/directional_light_component.hpp>
#include <engine/components/point_light_component.hpp>
#include <engine/components/static_mesh_component.hpp>
#include <content_browser/postprocess.hpp>

#include <core/serialization/archives/binary.hpp>
#include <core/serialization/serialization.hpp>
namespace anton_engine {
    static void load_world() {
        // #define RENDER_CUBES

        // asset_importing::import("C:/Users/An0num0us/Documents/GameEngine/assets_main/barrel_texture.png");
        // asset_importing::import("C:/Users/An0num0us/Documents/GameEngine/assets_main/barrel_normal_map.png");

        // {
        //     asset_importing::Imported_Meshes imported_meshes = asset_importing::import_mesh("C:/Users/An0num0us/Documents/GameEngine_Game/assets/barrel.obj");
        //     for (Mesh& mesh: imported_meshes.meshes) {
        //         flip_texture_coordinates(mesh.vertices);
        //     }
        //     u64 const barrel_guids[] = {1};
        //     asset_importing::save_meshes("barrel", barrel_guids, imported_meshes.meshes);
        // }
        // {
        //     asset_importing::Imported_Meshes imported_meshes = asset_importing::import_mesh("C:/Users/An0num0us/Documents/GameEngine_Game/assets/boxes1.obj");
        //     for (Mesh& mesh: imported_meshes.meshes) {
        //         flip_texture_coordinates(mesh.vertices);
        //     }
        //     u64 const boxes_guids[] = {2, 3, 4};
        //     asset_importing::save_meshes("boxes1", boxes_guids, imported_meshes.meshes);
        // }

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
        [[maybe_unused]] Handle<Shader> unlit_default_shader_handle = shader_manager->add(std::move(unlit_default_shader));

        // BS code to output anything on the screen
#ifdef RENDER_CUBES
        // atl::Vector<Mesh> meshes = assets::load_model("cube.obj");
        auto& container = meshes[0];
        Handle<Material> material_handle = material_manager->add(Material());
#else
        Mesh container = assets::load_mesh("barrel", 1);
        Material barrel_mat;
        {
            atl::Vector<uint8_t> pixels;
            Texture_Format const format = assets::load_texture_no_mipmaps("barrel_texture", 0, pixels);
            Texture handle;
            void* pix_data = pixels.data();
            rendering::load_textures_generate_mipmaps(format, 1, &pix_data, &handle);
            barrel_mat.diffuse_texture = handle;
            barrel_mat.specular_texture = Texture::default_black;
            barrel_mat.normal_map = Texture::default_normal_map;
        }
        Handle<Material> const material_handle = material_manager->add(std::move(barrel_mat));
#endif

        Mesh boxes1_mesh_0 = assets::load_mesh("boxes1", 2);
        Mesh boxes1_mesh_1 = assets::load_mesh("boxes1", 3);
        Mesh boxes1_mesh_2 = assets::load_mesh("boxes1", 4);

        Handle<Mesh> box_handle = mesh_manager->add(std::move(container));
        Handle<Mesh> quad_mesh = mesh_manager->add(generate_plane());
        Handle<Mesh> boxes1_mesh = mesh_manager->add(atl::move(boxes1_mesh_0));
        Handle<Mesh> boxes2_mesh = mesh_manager->add(atl::move(boxes1_mesh_1));
        Handle<Mesh> boxes3_mesh = mesh_manager->add(atl::move(boxes1_mesh_2));

#if DESERIALIZE
        std::filesystem::path serialization_in_path = utils::concat_paths(paths::project_directory(), "ecs.bin");
        std::ifstream file(serialization_in_path, std::ios::binary);
        serialization::Binary_Input_Archive in_archive(file);
        deserialize(in_archive, *ecs);
#else
        auto instantiate_entity = [default_shader_handle, material_handle](Handle<Mesh> mesh_handle, Vector3 position, float rotation = 0) {
            Entity box = ecs->create();
            ecs->add_component<Entity_Name>(box, u8"Box");
            Transform& box_t = ecs->add_component<Transform>(box);
            Static_Mesh_Component& box_sm = ecs->add_component<Static_Mesh_Component>(box);
            box_sm.mesh_handle = mesh_handle;
            box_sm.shader_handle = default_shader_handle;
            box_sm.material_handle = material_handle;
            box_t.translate(position);
            box_t.rotate(Vector3::forward, math::radians(rotation));
        };

        auto instantiate_box = [default_shader_handle, box_handle, material_handle](Vector3 position, float rotation = 0) {
            Entity box = ecs->create();
            ecs->add_component<Entity_Name>(box, u8"Box");
            Transform& box_t = ecs->add_component<Transform>(box);
            Static_Mesh_Component& box_sm = ecs->add_component<Static_Mesh_Component>(box);
            box_sm.mesh_handle = box_handle;
            box_sm.shader_handle = default_shader_handle;
            box_sm.material_handle = material_handle;
            box_t.translate(position);
            box_t.rotate(Vector3::forward, math::radians(rotation));
        };

        instantiate_entity(box_handle, {0, 0, -1});
        instantiate_entity(box_handle, {-5, 7, 2}, 55.0f);
        instantiate_entity(box_handle, {-3, -1, 4});
        instantiate_entity(box_handle, {0, -1, 4});

        instantiate_entity(boxes1_mesh, {1, 2, -2});
        instantiate_entity(boxes2_mesh, {-1, 2, 0});
        instantiate_entity(boxes3_mesh, {1, 2, 0});

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
            ecs->add_component<Entity_Name>(lamp, u8"Point Lamp");
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
} // namespace anton_engine
