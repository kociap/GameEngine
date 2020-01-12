#include <editor.hpp>

#include <build_config.hpp>
#include <camera_movement.hpp>
#include <components/camera.hpp>
#include <components/entity_name.hpp>
#include <components/transform.hpp>
#include <debug_hotkeys.hpp>
#include <diagnostic_macros.hpp>
#include <ecs/ecs.hpp>
#include <editor_window.hpp>
#include <engine.hpp>
#include <input.hpp>
#include <input/input_internal.hpp>
#include <material.hpp>
#include <mesh.hpp>
#include <paths.hpp>
#include <paths_internal.hpp>
#include <renderer.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>
#include <time/time_internal.hpp>
#include <utils/filesystem.hpp>

// TODO: Remove
#include <logging.hpp>
#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
    static bool _quit = false;

    // TODO: Temporarily
    static Editor_Window* editor_window = nullptr;
    static Resource_Manager<Mesh>* mesh_manager = nullptr;
    static Resource_Manager<Shader>* shader_manager = nullptr;
    static Resource_Manager<Material>* material_manager = nullptr;
    static ECS* ecs = nullptr;

    void quit() {
        _quit = true;
    }

    static void loop() {
        time_update();

        ecs->remove_requested_entities();
    }

    // TODO: Forward decl of load_world. Remove (eventually)
    static void load_world();

    static void init() {
        time_init();
        mesh_manager = new Resource_Manager<Mesh>();
        shader_manager = new Resource_Manager<Shader>();
        material_manager = new Resource_Manager<Material>();
        ecs = new ECS();

        load_world();
    }

    static void terminate() {
#if SERIALIZE_ON_QUIT
        std::filesystem::path serialization_out_path = utils::concat_paths(paths::project_directory(), "ecs.bin");
        std::ofstream file(serialization_out_path, std::ios::binary | std::ios::trunc);
        serialization::Binary_Output_Archive out_archive(file);
        serialize(out_archive, Engine::get_ecs());
#endif
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

    bool Editor::should_close() {
        return _quit;
    }

    void Editor::quit() {
        anton_engine::quit();
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

#include <asset_guid.hpp>
#include <asset_importing.hpp>
#include <assets.hpp>
#include <components/directional_light_component.hpp>
#include <components/point_light_component.hpp>
#include <components/static_mesh_component.hpp>
#include <postprocess.hpp>

#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>
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
        // anton_stl::Vector<Mesh> meshes = assets::load_model("cube.obj");
        auto& container = meshes[0];
        Handle<Material> material_handle = material_manager->add(Material());
#else
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
#endif

        Mesh boxes1_mesh_0 = assets::load_mesh("boxes1", 2);
        Mesh boxes1_mesh_1 = assets::load_mesh("boxes1", 3);
        Mesh boxes1_mesh_2 = assets::load_mesh("boxes1", 4);

        Handle<Mesh> box_handle = mesh_manager->add(std::move(container));
        Handle<Mesh> quad_mesh = mesh_manager->add(generate_plane());
        Handle<Mesh> boxes1_mesh = mesh_manager->add(anton_stl::move(boxes1_mesh_0));
        Handle<Mesh> boxes2_mesh = mesh_manager->add(anton_stl::move(boxes1_mesh_1));
        Handle<Mesh> boxes3_mesh = mesh_manager->add(anton_stl::move(boxes1_mesh_2));

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
