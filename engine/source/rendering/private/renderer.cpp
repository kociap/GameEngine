#include <renderer.hpp>

#include <anton_int.hpp>
#include <anton_stl/algorithm.hpp>
#include <anton_stl/utility.hpp>
#include <anton_stl/vector.hpp>
#include <assets.hpp>
#include <build_config.hpp>
#include <builtin_shaders.hpp>
#include <components/camera.hpp>
#include <components/directional_light_component.hpp>
#include <components/line_component.hpp>
#include <components/point_light_component.hpp>
#include <components/spot_light_component.hpp>
#include <components/static_mesh_component.hpp>
#include <components/transform.hpp>
#include <debug_macros.hpp> // CHECK_GL_ERRORS
#include <ecs/ecs.hpp>
#include <editor.hpp>
#include <engine.hpp>
#include <framebuffer.hpp>
#include <glad.hpp>
#include <handle.hpp>
#include <intrinsics.hpp>
#include <math/matrix4.hpp>
#include <math/transform.hpp>
#include <mesh.hpp>
#include <opengl.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>
#include <utils/enum.hpp>

#include <algorithm> // std::sort

namespace anton_engine {
    static Resource_Manager<Mesh>& get_mesh_manager() {
#if GE_WITH_EDITOR
        return Editor::get_mesh_manager();
#else
        return Engine::get_mesh_manager();
#endif
    }

    static Resource_Manager<Shader>& get_shader_manager() {
#if GE_WITH_EDITOR
        return Editor::get_shader_manager();
#else
        return Engine::get_shader_manager();
#endif
    }

    static Resource_Manager<Material>& get_material_manager() {
#if GE_WITH_EDITOR
        return Editor::get_material_manager();
#else
        return Engine::get_material_manager();
#endif
    }

    static ECS& get_ecs() {
#if GE_WITH_EDITOR
        return Editor::get_ecs();
#else
        return Engine::get_ecs();
#endif
    }
} // namespace anton_engine

namespace anton_engine::rendering {
    struct Point_Light_Data {
        alignas(16) Vector3 position;
        alignas(16) Color color;
        float intensity;
        float attentuation_constant;
        float attentuation_linear;
        float attentuation_quadratic;
        float diffuse_strength;
        float specular_strength;
    };

    struct Directional_Light_Data {
        alignas(16) Color color;
        alignas(16) Vector3 direction;
        float intensity;
        float diffuse_strength;
        float specular_strength;
    };

    struct Lighting_Data {
        alignas(16) Color ambient_color;
        float ambient_strentgh;
        float shininess; // TODO: Remove. Has basically no meaning since it should be per material and should be a different property.
        int point_lights_count;
        int directional_light_count;
        Point_Light_Data point_lights[16];
        Directional_Light_Data directional_lights[16];
    };

    // Buffer binding indices
    constexpr u32 lighting_data_binding = 0;
    constexpr u32 draw_matrix_data = 1;
    constexpr u32 draw_material_data_binding = 2;

    // Dynamic lights and environment data. Bound to binding 0 and 1 respectively.
    static u32 lighting_data_ubo = 0;

    // Persistently mapped vertex buffer for rendering geometry.
    static u32 vertex_buffer = 0;
    constexpr i64 vertex_buffer_vertex_count = 1048576;
    constexpr i64 vertex_buffer_size = vertex_buffer_vertex_count * sizeof(Vertex);
    constexpr i64 draw_id_count = 65536;
    constexpr i64 draw_id_buffer_size = draw_id_count * sizeof(u32);
    static void* mapped_vertex_buffer = nullptr;
    // By default filled with numbers 0 - draw_id_count.
    static void* mapped_draw_id_buffer = nullptr;

    constexpr i64 draw_elements_cmd_buffer_size = draw_id_count * sizeof(Draw_Elements_Command);
    constexpr i64 draw_arrays_cmd_buffer_size = draw_id_count * sizeof(Draw_Arrays_Command);
    constexpr i64 draw_cmd_buffer_size = draw_elements_cmd_buffer_size + draw_arrays_cmd_buffer_size;
    static u32 draw_cmd_buffer = 0;
    static void* mapped_draw_elements_cmd_buffer = nullptr;
    static void* mapped_draw_arrays_cmd_buffer = nullptr;

    // Persistently mapped SSBO with matrix data.
    static u32 ssbo = 0;
    constexpr i64 matrix_ssbo_size = draw_id_count * sizeof(Matrix4);
    constexpr i64 material_ssbo_size = draw_id_count * sizeof(Material);
    constexpr i64 ssbo_size = matrix_ssbo_size + material_ssbo_size;
    static void* mapped_matrix_ssbo = nullptr;
    static void* mapped_material_ssbo = nullptr;

    // Persistently mapped element buffer for rendering geometry.
    static u32 element_buffer = 0;
    constexpr i64 element_buffer_size = 1048576;
    static void* mapped_element_buffer = nullptr;

    struct Array_Texture {
        uint32_t handle;
        Texture_Format format;
    };

    [[nodiscard]] static bool operator==(Texture_Format lhs, Texture_Format rhs) {
        bool const swizzle_equal = lhs.swizzle_mask[0] == rhs.swizzle_mask[0] && lhs.swizzle_mask[1] == rhs.swizzle_mask[1] &&
                                   lhs.swizzle_mask[2] == rhs.swizzle_mask[2] && lhs.swizzle_mask[3] == rhs.swizzle_mask[3];
        return lhs.width == rhs.width && lhs.height == rhs.height && lhs.mip_levels == rhs.mip_levels &&
               lhs.sized_internal_format == rhs.sized_internal_format && swizzle_equal && lhs.pixel_type == rhs.pixel_type &&
               lhs.pixel_format == rhs.pixel_format;
    }

    [[nodiscard]] static bool operator!=(Texture_Format lhs, Texture_Format rhs) {
        return !(lhs == rhs);
    }

    struct Array_Texture_Storage {
        anton_stl::Vector<uint32_t> free_list;
        int32_t size = 0;
    };

    static anton_stl::Vector<Array_Texture> textures(64, Array_Texture{});
    static anton_stl::Vector<Array_Texture_Storage> textures_storage(64, Array_Texture_Storage());

    // Draw command buffers.
    static anton_stl::Vector<Draw_Arrays_Command> draw_arrays_commands;
    static anton_stl::Vector<Draw_Elements_Command> draw_elements_commands;

    // Standard vao used for rendering meshes with position, normals, texture coords, tangent and bitangent.
    // Uses binding index 0.
    static uint32_t mesh_vao = 0;

    constexpr uint64_t align_size(uint64_t const size, uint64_t const alignment) {
        uint64_t const misalignment = size & (alignment - 1);
        return size + (misalignment != 0 ? alignment - misalignment : 0);
    }

    // static uint64_t align_size(uint64_t const size, uint64_t const alignment) {
    //     return (size + alignment - 1) / alignment * alignment;
    // }

    void setup_rendering() {
        glDisable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenVertexArrays(1, &mesh_vao);
        glBindVertexArray(mesh_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, false, offsetof(Vertex, position));
        glVertexAttribBinding(0, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 3, GL_FLOAT, false, offsetof(Vertex, normal));
        glVertexAttribBinding(1, 0);
        glEnableVertexAttribArray(2);
        glVertexAttribFormat(2, 3, GL_FLOAT, false, offsetof(Vertex, tangent));
        glVertexAttribBinding(2, 0);
        glEnableVertexAttribArray(3);
        glVertexAttribFormat(3, 3, GL_FLOAT, false, offsetof(Vertex, bitangent));
        glVertexAttribBinding(3, 0);
        glEnableVertexAttribArray(4);
        glVertexAttribFormat(4, 2, GL_FLOAT, false, offsetof(Vertex, uv_coordinates));
        glVertexAttribBinding(4, 0);
        glEnableVertexAttribArray(5);
        glVertexAttribIFormat(5, 1, GL_UNSIGNED_INT, 0);
        glVertexAttribBinding(5, 1);
        glVertexBindingDivisor(1, 1);

        constexpr u32 buffer_flags = GL_MAP_COHERENT_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

        // TODO: Synchronisation to prevent buffer races
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        // Huge buffer
        glBufferStorage(GL_ARRAY_BUFFER, vertex_buffer_size + draw_id_buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        glBindVertexBuffer(0, vertex_buffer, 0, sizeof(Vertex));
        glBindVertexBuffer(1, vertex_buffer, vertex_buffer_size, sizeof(u32));
        mapped_vertex_buffer = glMapBufferRange(GL_ARRAY_BUFFER, 0, vertex_buffer_size, buffer_flags);
        mapped_draw_id_buffer = reinterpret_cast<char*>(mapped_vertex_buffer) + vertex_buffer_size;
        anton_stl::iota(reinterpret_cast<u32*>(mapped_draw_id_buffer), reinterpret_cast<u32*>(mapped_draw_id_buffer) + draw_id_count, 0);

        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, ssbo_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, draw_matrix_data, ssbo, 0, matrix_ssbo_size);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, draw_material_data_binding, ssbo, matrix_ssbo_size, material_ssbo_size);
        CHECK_GL_ERRORS();
        mapped_matrix_ssbo = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ssbo_size, buffer_flags);
        mapped_material_ssbo = reinterpret_cast<char*>(mapped_matrix_ssbo) + matrix_ssbo_size;
        CHECK_GL_ERRORS();

        glGenBuffers(2, &element_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        // Another huge buffer
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        mapped_element_buffer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, element_buffer_size, buffer_flags);

        // Indirect Draw CMD buffer
        glGenBuffers(1, &draw_cmd_buffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, draw_cmd_buffer);
        glBufferStorage(GL_DRAW_INDIRECT_BUFFER, draw_cmd_buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        mapped_draw_elements_cmd_buffer = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, draw_cmd_buffer_size, buffer_flags);
        mapped_draw_arrays_cmd_buffer = reinterpret_cast<char*>(mapped_draw_elements_cmd_buffer) + draw_elements_cmd_buffer_size;

        // Uniforms

        glGenBuffers(1, &lighting_data_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, lighting_data_ubo);
        glBufferStorage(GL_UNIFORM_BUFFER, sizeof(Lighting_Data), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, lighting_data_binding, lighting_data_ubo, 0, sizeof(Lighting_Data));

        // Default Textures

        // Empty (0, 0, 0, 1) 1x1 texture bound to unit 0 layer 0
        // Default normal map (0.5, 0.5, 1.0, 1.0) 1x1  bound to unit 0 layer 1
        {
            Texture_Format const default_format = {
                1, 1, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST_MIPMAP_NEAREST, 1, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}};
            uint8_t const pixels[] = {0, 0, 0, 127, 127, 255};
            void const* const pixels_loc[2] = {pixels, pixels + 3};
            Texture handles[2];
            // TODO: Something's wrong with loading to gpu since the textures don't appear in renderdoc
            load_textures_generate_mipmaps(default_format, 2, pixels_loc, handles);
            bind_texture(0, handles[0]);
        }
    }

    void update_dynamic_lights() {
        ECS& ecs = get_ecs();
        // TODO: We load hardcoded environment properties at startup, but they should be modifiable.
        Lighting_Data lights_data = {{1.0f, 1.0f, 1.0f}, 0.02f, 32.0f, 0, 0, {}, {}};
        {
            auto directional_lights = ecs.view<Directional_Light_Component>();
            lights_data.directional_light_count = directional_lights.size();
            int32_t i = 0;
            for (Entity const entity: directional_lights) {
                Directional_Light_Component& light = directional_lights.get(entity);
                lights_data.directional_lights[i] = {light.color, light.direction, light.intensity, 0.8f, 1.0f};
                ++i;
            }
        }
        // {
        //     auto spot_lights = ecs.view<Transform, Spot_Light_Component>();
        //     int32_t i = 0;
        //     for (Entity const entity: spot_lights) {
        //         auto [transform, light] = spot_lights.get<Transform, Spot_Light_Component>(entity);
        //         light_data.spot_lights[i] = {transform.local_position, light.direction, light.color, light.intensity, light.cutoff_angle, light.blend_angle,
        //                                      1.0f, 0.09f, 0.032f, 0.8f, 1.0f};
        //         ++i;
        //     }
        // }
        {
            auto point_lights = ecs.view<Transform, Point_Light_Component>();
            lights_data.point_lights_count = point_lights.size();
            int32_t i = 0;
            for (Entity const entity: point_lights) {
                auto [transform, light] = point_lights.get<Transform, Point_Light_Component>(entity);
                // TODO: Global attentuation instead of per light. Most likely hardcoded in the shaders
                lights_data.point_lights[i] = {transform.local_position, light.color, light.intensity, 1.0f, 0.09f, 0.032f, 0.8f, 1.0f};
                ++i;
            }
        }

        glNamedBufferSubData(lighting_data_ubo, 0, sizeof(Lighting_Data), &lights_data);
    }

    void bind_mesh_vao() {
        glBindVertexArray(mesh_vao);
    }

    Vertex_Buffer get_vertex_buffer() {
        return {mapped_vertex_buffer, vertex_buffer_size};
    }

    Draw_ID_Buffer get_draw_id_buffer() {
        return {reinterpret_cast<u32*>(mapped_draw_id_buffer), draw_id_buffer_size};
    }

    Matrix_Buffer get_matrix_buffer() {
        return {reinterpret_cast<Matrix4*>(mapped_matrix_ssbo), ssbo_size};
    }

    Material_Buffer get_material_buffer() {
        return {reinterpret_cast<Material*>(mapped_material_ssbo), material_ssbo_size};
    }

    Element_Buffer get_element_buffer() {
        return {reinterpret_cast<u32*>(mapped_element_buffer), element_buffer_size};
    }

    [[nodiscard]] static int32_t find_texture_with_format(Texture_Format const format) {
        for (int32_t i = 0; i < textures.size(); ++i) {
            if (textures[i].format == format) {
                return i;
            }
        }

        return -1;
    }

    [[nodiscard]] static int32_t find_unused_texture() {
        for (int32_t i = 0; i < textures.size(); ++i) {
            if (textures[i].handle == 0) {
                return i;
            }
        }

        int32_t unused = textures.size();
        textures.resize(textures.size() * 2);
        return unused;
    }

    [[nodiscard]] static bool should_use_linear_magnififcation_filter(GLenum const min_filter) {
        return min_filter == GL_LINEAR || min_filter == GL_LINEAR_MIPMAP_NEAREST || min_filter == GL_LINEAR_MIPMAP_LINEAR;
    }

    // Handle <gl texture handle (u32), layer (u32)>
    void load_textures_generate_mipmaps(Texture_Format const format, int32_t const texture_count, void const* const* const pixels, Texture* const handles) {
        int32_t texture_index = find_texture_with_format(format);
        if (texture_index == -1) {
            int32_t unused_texture = find_unused_texture();
            textures[unused_texture].format = format;
            glGenTextures(1, &textures[unused_texture].handle);
            glBindTexture(GL_TEXTURE_2D_ARRAY, textures[unused_texture].handle);
            glTexStorage3D(GL_TEXTURE_2D_ARRAY, format.mip_levels, format.sized_internal_format, format.width, format.height, texture_count);
            // uint32_t const mag_filter = should_use_linear_magnififcation_filter(texture.filter) ? GL_LINEAR : GL_NEAREST;
            // TODO: Currently all textures are trilinearily filtered by default.
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, format.swizzle_mask);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            for (int32_t i = 0; i < texture_count; ++i) {
                textures_storage[unused_texture].free_list.push_back(i);
            }
            texture_index = unused_texture;
        } else {
            if (textures_storage[texture_index].free_list.size() >= texture_count) {
                glBindTexture(GL_TEXTURE_2D_ARRAY, textures[texture_index].handle);
            } else {
                uint32_t new_texture;
                glGenTextures(1, &new_texture);
                glBindTexture(GL_TEXTURE_2D_ARRAY, new_texture);
                Array_Texture_Storage& texture_storage = textures_storage[texture_index];
                int32_t const new_size = texture_storage.size + texture_count - texture_storage.free_list.size();
                glTexStorage3D(GL_TEXTURE_2D_ARRAY, format.mip_levels, format.sized_internal_format, format.width, format.height, new_size);
                for (int32_t i = 0; i < format.mip_levels; ++i) {
                    glCopyImageSubData(textures[texture_index].handle, GL_TEXTURE_2D_ARRAY, i, 0, 0, 0, new_texture, GL_TEXTURE_2D_ARRAY, i, 0, 0, 0,
                                       format.width, format.height, texture_storage.size);
                }
                glDeleteTextures(1, &textures[texture_index].handle);
                textures[texture_index].handle = new_texture;
                int32_t const old_size = texture_storage.size;
                texture_storage.size = new_size;
                for (int32_t i = old_size; i < new_size; ++i) {
                    texture_storage.free_list.push_back(i);
                }
            }
        }

        Array_Texture_Storage& texture_storage = textures_storage[texture_index];
        for (int32_t i = 0; i < texture_count; ++i) {
            int32_t const unused_texture = texture_storage.free_list[i];
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, unused_texture, format.width, format.height, 1, format.pixel_format, format.pixel_type, pixels[i]);
            handles[i].index = texture_index;
            handles[i].layer = unused_texture;
        }
        texture_storage.free_list.erase(texture_storage.free_list.begin(), texture_storage.free_list.begin() + texture_count);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        CHECK_GL_ERRORS();
    }

    static void bind_default_textures() {
        bind_texture(0, Texture{0, 0});
    }

    void bind_texture(uint32_t const unit, Texture const handle) {
        glBindTextureUnit(unit, textures[handle.index].handle);
    }

    // void unload_texture(uint64_t const handle) {
    //     uint32_t const texture_index = handle >> 32;
    //     uint32_t const level = handle & 0xFFFFFFFF;
    //     ANTON_ASSERT(texture_index < textures.size(), "Invalid texture index.");
    //     ANTON_ASSERT(level < textures[texture_index].size(), "Invalid texture level.");
    //     textures[texture_index].free_list.append(level);
    // }

    // void unload_textures(int32_t handle_count, uint64_t const* handles) {
    //     ANTON_ASSERT(handle_count >= 0, "Handle count may not be less than 0.");
    //     for (int32_t i = 0; i < handle_count; ++i) {}
    // }

    void add_draw_command(Draw_Arrays_Command const command) {
        draw_arrays_commands.push_back(command);
    }

    void add_draw_command(Draw_Elements_Command const command) {
        draw_elements_commands.push_back(command);
    }

    void commit_draw() {
        if (draw_elements_commands.size() > 0) {
            memcpy(mapped_draw_elements_cmd_buffer, draw_elements_commands.data(), draw_elements_commands.size() * sizeof(Draw_Elements_Command));
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, draw_elements_commands.size(), sizeof(Draw_Elements_Command));
            draw_elements_commands.clear();
        }

        if (draw_arrays_commands.size() > 0) {
            memcpy(mapped_draw_arrays_cmd_buffer, draw_arrays_commands.data(), draw_arrays_commands.size() * sizeof(Draw_Arrays_Command));
            glMultiDrawArraysIndirect(GL_TRIANGLES, (void*)draw_elements_cmd_buffer_size, draw_arrays_commands.size(), sizeof(Draw_Arrays_Command));
            draw_arrays_commands.clear();
        }
    }

    void render_scene(ECS snapshot, Transform const camera_transform, Matrix4 const view, Matrix4 const projection) {
        snapshot.sort<Static_Mesh_Component>(
            [](auto begin, auto end, auto predicate) { std::sort(begin, end, predicate); },
            [](Static_Mesh_Component const lhs, Static_Mesh_Component const rhs) -> bool {
                return lhs.shader_handle < rhs.shader_handle || (lhs.shader_handle == rhs.shader_handle && lhs.material_handle < rhs.material_handle) ||
                       (lhs.shader_handle == rhs.shader_handle && lhs.material_handle == rhs.material_handle && lhs.mesh_handle < rhs.mesh_handle);
            });

        // snapshot.respect<Static_Mesh_Component, Transform>();
        bind_default_textures();
        bind_mesh_vao();
        glBindVertexBuffer(0, vertex_buffer, 0, sizeof(Vertex));
        glBindVertexBuffer(1, vertex_buffer, vertex_buffer_size, sizeof(u32));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        auto objects = snapshot.view<Static_Mesh_Component, Transform>();
        Static_Mesh_Component last_mesh = {};
        Resource_Manager<Shader>& shader_manager = get_shader_manager();
        Resource_Manager<Material>& material_manager = get_material_manager();
        Resource_Manager<Mesh>& mesh_manager = get_mesh_manager();
        Mapped_Buffer material_buffer = get_material_buffer();
        Mapped_Buffer vertex_buffer = get_vertex_buffer();
        Mapped_Buffer element_buffer = get_element_buffer();
        Mapped_Buffer matrix_buffer = get_matrix_buffer();

        struct Bound_Texture {
            u32 index = 0;
            i64 draw = -1;
        };

        auto find_slot_and_bind_texture = [](Bound_Texture bound_textures[16], Texture const texture, i64 const current_draw) -> u32 {
            for (i32 i = 1; i < 16; ++i) {
                if (bound_textures[i].index == texture.index) {
                    return i;
                }
            }

            i32 max_draw_index = 0;
            i64 max_draw = -1;
            for (i32 i = 1; i < 16; ++i) {
                if (bound_textures[i].draw == -1) {
                    bind_texture(i, texture);
                    bound_textures[i] = Bound_Texture{texture.index, current_draw};
                    return i;
                } else if (bound_textures[i].draw > max_draw) {
                    max_draw_index = i;
                    max_draw = bound_textures[i].draw;
                }
            }

            bind_texture(max_draw_index, texture);
            bound_textures[max_draw_index] = Bound_Texture{texture.index, current_draw};
            return max_draw_index;
        };

        Bound_Texture bound_textures[16] = {};
        i64 vertices_written = 0;
        i64 indices_written = 0;
        Vertex* vertex_head_ptr = reinterpret_cast<Vertex*>(vertex_buffer.data);
        u32* index_head_ptr = element_buffer.data;
        Material* material_head_ptr = material_buffer.data;
        Matrix4* matrix_head_ptr = matrix_buffer.data;
        i64 current_draw = 0;
        Draw_Elements_Command cmd = {};
        // Fairly dumb rendering loop.
        for (auto iter = objects.begin(), end = objects.end(); iter != end; ++iter) {
            auto const [transform, static_mesh] = objects.get<Transform, Static_Mesh_Component>(*iter);
            if (static_mesh.shader_handle != last_mesh.shader_handle || static_mesh.mesh_handle != last_mesh.mesh_handle ||
                static_mesh.material_handle != last_mesh.material_handle) {
                if (ANTON_LIKELY(current_draw != 0)) {
                    add_draw_command(cmd);
                }
                cmd.base_instance = current_draw;
                cmd.instance_count = 1;
            } else {
                cmd.instance_count += 1;
            }

            if (static_mesh.shader_handle != last_mesh.shader_handle) {
                if (ANTON_LIKELY(current_draw != 0)) {
                    commit_draw();
                }
                Shader& shader = shader_manager.get(static_mesh.shader_handle);
                shader.use();
                shader.set_vec3("camera.position", camera_transform.local_position);
                shader.set_matrix4("projection", projection);
                shader.set_matrix4("view", view);
            }

            if (static_mesh.mesh_handle != last_mesh.mesh_handle) {
                Mesh const& mesh = mesh_manager.get(static_mesh.mesh_handle);
                i64 const vertex_data_size = mesh.vertices.size();
                memcpy(vertex_head_ptr, mesh.vertices.data(), vertex_data_size * sizeof(Vertex));
                vertex_head_ptr += vertex_data_size;
                cmd.base_vertex = vertices_written;
                vertices_written += vertex_data_size;
                i64 const indices_data_size = mesh.indices.size();
                memcpy(index_head_ptr, mesh.indices.data(), indices_data_size * sizeof(u32));
                index_head_ptr += indices_data_size;
                cmd.count = mesh.indices.size();
                cmd.first_index = indices_written;
                indices_written += indices_data_size;
            }

            {
                // TODO: Does pointless calls to find_slot_and_bind_texture even though
                //       it doesn't actually rebind textures each iteration.
                //
                // Skip first texture slot since it should always have the default textures bound
                Material const mat = material_manager.get(static_mesh.material_handle);
                Material shader_mat = mat;
                shader_mat.diffuse_texture.index = find_slot_and_bind_texture(bound_textures, mat.diffuse_texture, current_draw);
                shader_mat.specular_texture.index = find_slot_and_bind_texture(bound_textures, mat.specular_texture, current_draw);
                shader_mat.normal_map.index = find_slot_and_bind_texture(bound_textures, mat.normal_map, current_draw);
                // memcpy(material_head_ptr, &shader_mat, sizeof(Material));
                *material_head_ptr = shader_mat;
                material_head_ptr += 1;
            }

            {
                Matrix4 const model = to_matrix(transform);
                // memcpy(matrix_head_ptr, &model, sizeof(Matrix4));
                *matrix_head_ptr = model;
                matrix_head_ptr += 1;
            }

            last_mesh = static_mesh;
            current_draw += 1;
        }

        // Since there's no state change at the end of the loop we have to
        // add the last draw command and kick rendering off manually.
        add_draw_command(cmd);
        commit_draw();
    }

    Renderer::Renderer(int32_t width, int32_t height) {
        build_framebuffers(width, height);
    }

    Renderer::~Renderer() {
        delete_framebuffers();
    }

    void Renderer::build_framebuffers(int32_t width, int32_t height) {
        Framebuffer::Construct_Info framebuffer_info;
        framebuffer_info.width = width;
        framebuffer_info.height = height;
        framebuffer_info.depth_buffer.enabled = true;
        framebuffer_info.color_buffers.resize(3);
        // Position
        framebuffer_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgb32f;
        // Normal
        framebuffer_info.color_buffers[1].internal_format = Framebuffer::Internal_Format::rgb32f;
        // albedo-specular
        framebuffer_info.color_buffers[2].internal_format = Framebuffer::Internal_Format::rgba8;
        framebuffer = new Framebuffer(framebuffer_info);

        Framebuffer::Construct_Info postprocess_construct_info;
        postprocess_construct_info.color_buffers.resize(1);
        postprocess_construct_info.width = width;
        postprocess_construct_info.height = height;
        postprocess_back_buffer = new Framebuffer(postprocess_construct_info);
        postprocess_front_buffer = new Framebuffer(postprocess_construct_info);
    }

    void Renderer::delete_framebuffers() {
        delete framebuffer;
        delete postprocess_back_buffer;
        delete postprocess_front_buffer;
    }

    void Renderer::resize(int32_t width, int32_t height) {
        delete_framebuffers();
        build_framebuffers(width, height);
    }

    void Renderer::swap_postprocess_buffers() {
        anton_stl::swap(postprocess_front_buffer, postprocess_back_buffer);
    }

    void Renderer::render_scene(Transform const camera_transform, Matrix4 const view, Matrix4 const projection) {
        ECS& ecs = get_ecs();
        ECS snapshot = ecs.snapshot<Transform, Static_Mesh_Component>();
        // cull_frustum();
        ::anton_engine::rendering::render_scene(snapshot, camera_transform, view, projection);
    }

    uint32_t Renderer::render_frame_as_texture(Matrix4 const view_mat, Matrix4 const proj_mat, Transform const camera_transform, int32_t const viewport_width,
                                               int32_t const viewport_height) {
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, viewport_width, viewport_height);
        Framebuffer::bind(*framebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bind_mesh_vao();
        render_scene(camera_transform, view_mat, proj_mat);

        // Postprocessing

        glDisable(GL_DEPTH_TEST);
        Framebuffer::bind(*postprocess_back_buffer);
        glBindTextureUnit(0, framebuffer->get_color_texture(0));
        glBindTextureUnit(1, framebuffer->get_color_texture(1));
        glBindTextureUnit(2, framebuffer->get_color_texture(2));

        Shader& deferred_shading = get_builtin_shader(Builtin_Shader::deferred_shading);
        deferred_shading.use();
        deferred_shading.set_vec3("camera.position", camera_transform.local_position);
        render_texture_quad();
        glEnable(GL_DEPTH_TEST);
        swap_postprocess_buffers();
        return postprocess_front_buffer->get_color_texture(0);
    }

    void Renderer::render_frame(Matrix4 const view_mat, Matrix4 const proj_mat, Transform const camera_transform, int32_t const viewport_width,
                                int32_t const viewport_height) {
        uint32_t frame_texture = render_frame_as_texture(view_mat, proj_mat, camera_transform, viewport_width, viewport_height);
        Framebuffer::bind_default();
        opengl::active_texture(0);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, frame_texture);
        Shader& gamma_correction_shader = get_builtin_shader(Builtin_Shader::gamma_correction);
        gamma_correction_shader.use();
        gamma_correction_shader.set_float("gamma", 1 / 2.2f);
        render_texture_quad();
    }

    // TODO: Temporary solution for rendering the texture quad

    static u32 gen_texture_quad_buffers(Mesh const mesh) {
        u32 vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferStorage(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), mesh.vertices.data(), 0);
        return vbo;
    }

    void render_texture_quad() {
        u32 const static vbo = gen_texture_quad_buffers(generate_plane());
        glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
} // namespace anton_engine::rendering
