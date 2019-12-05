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
#include <unordered_map>

#if ANTON_WITH_EDITOR
#    include <editor.hpp>
#endif

namespace anton_engine {
    static Resource_Manager<Mesh>& get_mesh_manager() {
#if ANTON_WITH_EDITOR
        return Editor::get_mesh_manager();
#else
        return Engine::get_mesh_manager();
#endif
    }

    static Resource_Manager<Shader>& get_shader_manager() {
#if ANTON_WITH_EDITOR
        return Editor::get_shader_manager();
#else
        return Engine::get_shader_manager();
#endif
    }

    static Resource_Manager<Material>& get_material_manager() {
#if ANTON_WITH_EDITOR
        return Editor::get_material_manager();
#else
        return Engine::get_material_manager();
#endif
    }

    static ECS& get_ecs() {
#if ANTON_WITH_EDITOR
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

    struct GPU_Buffer {
        u32 handle;
        void* mapped;
        i64 size;
    };

    // Buffer binding indices
    constexpr u32 lighting_data_binding = 0;
    constexpr u32 draw_matrix_data = 1;
    constexpr u32 draw_material_data_binding = 2;

    // Dynamic lights and environment data. Bound to binding 0 and 1 respectively.
    static u32 lighting_data_ubo = 0;

    constexpr i64 vertex_buffer_vertex_count = 1048576 * 2;
    constexpr i64 vertex_buffer_size = vertex_buffer_vertex_count * sizeof(Vertex);
    static GPU_Buffer gpu_vertex_buffer;
    static Buffer<Vertex> vertex_buffer;
    static Buffer<Vertex> persistent_vertex_buffer;

    constexpr i64 draw_id_count = 65536;
    constexpr i64 draw_id_buffer_size = draw_id_count * sizeof(u32);
    // By default filled with numbers 0 - draw_id_count.
    static Buffer<u32> draw_id_buffer;

    constexpr i64 draw_cmd_buffer_size = draw_id_count * sizeof(Draw_Elements_Command);
    static GPU_Buffer gpu_draw_cmd_buffer;
    static Buffer<Draw_Elements_Command> draw_elements_cmd_buffer;

    // Persistently mapped SSBO with matrix data.
    constexpr i64 matrix_ssbo_size = draw_id_count * sizeof(Matrix4);
    constexpr i64 material_ssbo_size = draw_id_count * sizeof(Material);
    constexpr i64 ssbo_size = matrix_ssbo_size + material_ssbo_size;
    static GPU_Buffer gpu_ssbo;
    static Buffer<Matrix4> matrix_buffer;
    static Buffer<Material> material_buffer;

    // Persistently mapped element buffer for rendering geometry.
    constexpr i64 element_buffer_index_count = 1048576 * 2;
    constexpr i64 element_buffer_size = element_buffer_index_count * sizeof(u32);
    static GPU_Buffer gpu_element_buffer;
    static Buffer<u32> element_buffer;
    static Buffer<u32> persistent_element_buffer;

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

    struct Array_Texture {
        u32 handle;
        Texture_Format format;
    };

    struct Array_Texture_Storage {
        anton_stl::Vector<uint32_t> free_list;
        i32 size = 0;
    };

    static anton_stl::Vector<Array_Texture> textures(64, Array_Texture{});
    static anton_stl::Vector<Array_Texture_Storage> textures_storage(64, Array_Texture_Storage());

    // Draw commands buffer
    static anton_stl::Vector<Draw_Elements_Command> draw_elements_commands;

    static std::unordered_map<u64, Draw_Elements_Command> persistent_draw_commands_map;

    static u64 get_persistent_geometry_next_handle() {
        static u64 handle = 0;
        return ++handle;
    }

    // Standard vao used for rendering meshes with position, normals, texture coords, tangent and bitangent.
    // Uses binding index 0.
    static u32 mesh_vao = 0;

    constexpr u64 align_size(u64 const size, u64 const alignment) {
        u64 const misalignment = size & (alignment - 1);
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
        // TODO: One huge buffer for (almost) everything (I guess)

        glGenBuffers(1, &gpu_vertex_buffer.handle);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_vertex_buffer.handle);
        glBufferStorage(GL_ARRAY_BUFFER, vertex_buffer_size + draw_id_buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        gpu_vertex_buffer.size = vertex_buffer_size;
        glBindVertexBuffer(0, gpu_vertex_buffer.handle, 0, sizeof(Vertex));
        glBindVertexBuffer(1, gpu_vertex_buffer.handle, vertex_buffer_size, sizeof(u32));

        gpu_vertex_buffer.mapped = glMapBufferRange(GL_ARRAY_BUFFER, 0, vertex_buffer_size + draw_id_buffer_size, buffer_flags);
        vertex_buffer.buffer = vertex_buffer.head = reinterpret_cast<Vertex*>(gpu_vertex_buffer.mapped);
        vertex_buffer.size = vertex_buffer_vertex_count / 2;
        persistent_vertex_buffer.buffer = persistent_vertex_buffer.head = vertex_buffer.buffer + vertex_buffer_vertex_count / 2;
        persistent_vertex_buffer.size = vertex_buffer_vertex_count / 2;

        draw_id_buffer.buffer = draw_id_buffer.head = reinterpret_cast<u32*>(reinterpret_cast<char*>(gpu_vertex_buffer.mapped) + vertex_buffer_size);
        anton_stl::iota(draw_id_buffer.buffer, draw_id_buffer.buffer + draw_id_count, 0);

        glGenBuffers(1, &gpu_ssbo.handle);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, gpu_ssbo.handle);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, ssbo_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        gpu_ssbo.size = ssbo_size;
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, draw_matrix_data, gpu_ssbo.handle, 0, matrix_ssbo_size);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, draw_material_data_binding, gpu_ssbo.handle, matrix_ssbo_size, material_ssbo_size);
        gpu_ssbo.mapped = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ssbo_size, buffer_flags);
        matrix_buffer.buffer = matrix_buffer.head = reinterpret_cast<Matrix4*>(gpu_ssbo.mapped);
        matrix_buffer.size = matrix_ssbo_size / sizeof(Matrix4);
        material_buffer.buffer = material_buffer.head = reinterpret_cast<Material*>(matrix_buffer.buffer + matrix_ssbo_size / sizeof(Matrix4));
        material_buffer.size = material_ssbo_size / sizeof(Material);

        glGenBuffers(2, &gpu_element_buffer.handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_element_buffer.handle);
        // Another huge buffer
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        gpu_element_buffer.size = element_buffer_size;
        gpu_element_buffer.mapped = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, element_buffer_size, buffer_flags);
        element_buffer.buffer = element_buffer.head = reinterpret_cast<u32*>(gpu_element_buffer.mapped);
        element_buffer.size = element_buffer_index_count / 2;
        persistent_element_buffer.buffer = persistent_element_buffer.head = element_buffer.buffer + element_buffer_index_count / 2;
        persistent_element_buffer.size = element_buffer_index_count / 2;

        // Indirect Draw CMD buffer
        glGenBuffers(1, &gpu_draw_cmd_buffer.handle);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gpu_draw_cmd_buffer.handle);
        glBufferStorage(GL_DRAW_INDIRECT_BUFFER, draw_cmd_buffer_size, nullptr, GL_DYNAMIC_STORAGE_BIT | buffer_flags);
        gpu_draw_cmd_buffer.size = draw_cmd_buffer_size;
        gpu_draw_cmd_buffer.mapped = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, draw_cmd_buffer_size, buffer_flags);
        draw_elements_cmd_buffer.buffer = draw_elements_cmd_buffer.head = reinterpret_cast<Draw_Elements_Command*>(gpu_draw_cmd_buffer.mapped);
        draw_elements_cmd_buffer.size = gpu_draw_cmd_buffer.size / sizeof(Draw_Elements_Command);

        // Uniforms

        glGenBuffers(1, &lighting_data_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, lighting_data_ubo);
        glBufferStorage(GL_UNIFORM_BUFFER, sizeof(Lighting_Data), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, lighting_data_binding, lighting_data_ubo, 0, sizeof(Lighting_Data));

        // Default Textures

        // Black (0, 0, 0, 1) 1x1 texture bound to unit 0 layer 0
        // Black transparent (0, 0, 0, 0) 1x1 texture bound to unit 0 layer 1
        // Default tangent space normal map (0.5, 0.5, 1.0, 1.0) 1x1 bound to unit 0 layer 2
        {
            Texture_Format const default_format = {
                1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST_MIPMAP_NEAREST, 1, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}};
            u8 const pixels[] = {0, 0, 0, 255, 127, 127, 255};
            void const* const pixels_loc[2] = {pixels, pixels + 3};
            Texture handles[2];
            // TODO: Something's wrong with loading to gpu since the textures don't appear in renderdoc
            //  Are you sure about that?
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

    void bind_vertex_buffers() {
        glBindVertexBuffer(0, gpu_vertex_buffer.handle, 0, sizeof(Vertex));
        glBindVertexBuffer(1, gpu_vertex_buffer.handle, vertex_buffer_size, sizeof(u32));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_element_buffer.handle);
    }

    Buffer<Vertex>& get_vertex_buffer() {
        return vertex_buffer;
    }

    Buffer<u32>& get_draw_id_buffer() {
        return draw_id_buffer;
    }

    Buffer<Matrix4>& get_matrix_buffer() {
        return matrix_buffer;
    }

    Buffer<Material>& get_material_buffer() {
        return material_buffer;
    }

    Buffer<u32>& get_element_buffer() {
        return element_buffer;
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
        i32 texture_index = find_texture_with_format(format);
        if (texture_index == -1) {
            i32 unused_texture = find_unused_texture();
            textures[unused_texture].format = format;
            glGenTextures(1, &textures[unused_texture].handle);
            glBindTexture(GL_TEXTURE_2D_ARRAY, textures[unused_texture].handle);
            glTexStorage3D(GL_TEXTURE_2D_ARRAY, format.mip_levels, format.sized_internal_format, format.width, format.height, texture_count);
            // u32 const mag_filter = should_use_linear_magnififcation_filter(texture.filter) ? GL_LINEAR : GL_NEAREST;
            // TODO: Currently all textures are trilinearily filtered by default.
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, format.swizzle_mask);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            for (i32 i = 0; i < texture_count; ++i) {
                textures_storage[unused_texture].free_list.push_back(i);
            }
            texture_index = unused_texture;
        } else {
            if (textures_storage[texture_index].free_list.size() >= texture_count) {
                glBindTexture(GL_TEXTURE_2D_ARRAY, textures[texture_index].handle);
            } else {
                u32 new_texture;
                glGenTextures(1, &new_texture);
                glBindTexture(GL_TEXTURE_2D_ARRAY, new_texture);
                Array_Texture_Storage& texture_storage = textures_storage[texture_index];
                i32 const new_size = texture_storage.size + texture_count - texture_storage.free_list.size();
                glTexStorage3D(GL_TEXTURE_2D_ARRAY, format.mip_levels, format.sized_internal_format, format.width, format.height, new_size);
                for (i32 i = 0; i < format.mip_levels; ++i) {
                    glCopyImageSubData(textures[texture_index].handle, GL_TEXTURE_2D_ARRAY, i, 0, 0, 0, new_texture, GL_TEXTURE_2D_ARRAY, i, 0, 0, 0,
                                       format.width, format.height, texture_storage.size);
                }
                glDeleteTextures(1, &textures[texture_index].handle);
                textures[texture_index].handle = new_texture;
                i32 const old_size = texture_storage.size;
                texture_storage.size = new_size;
                for (i32 i = old_size; i < new_size; ++i) {
                    texture_storage.free_list.push_back(i);
                }
            }
        }

        Array_Texture_Storage& texture_storage = textures_storage[texture_index];
        for (i32 i = 0; i < texture_count; ++i) {
            i32 const unused_texture = texture_storage.free_list[i];
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, unused_texture, format.width, format.height, 1, format.pixel_format, format.pixel_type, pixels[i]);
            handles[i].index = texture_index;
            handles[i].layer = unused_texture;
        }
        texture_storage.free_list.erase(texture_storage.free_list.begin(), texture_storage.free_list.begin() + texture_count);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    Draw_Elements_Command write_geometry(anton_stl::Slice<Vertex const> const vertices, anton_stl::Slice<u32 const> const indices) {
        Draw_Elements_Command cmd = {};

        i64 vert_head_offset = vertex_buffer.head - vertex_buffer.buffer;
        if (vertex_buffer.size - vert_head_offset < vertices.size()) {
            vertex_buffer.head = vertex_buffer.buffer;
            vert_head_offset = 0;
        }
        memcpy(vertex_buffer.head, vertices.data(), vertices.size() * sizeof(Vertex));
        vertex_buffer.head += vertices.size();
        cmd.base_vertex = vert_head_offset;

        i64 elem_head_offset = element_buffer.head - element_buffer.buffer;
        if (element_buffer.size - elem_head_offset < indices.size()) {
            element_buffer.head = element_buffer.buffer;
            elem_head_offset = 0;
        }
        memcpy(element_buffer.head, indices.data(), indices.size() * sizeof(u32));
        element_buffer.head += indices.size();
        cmd.count = indices.size();
        cmd.first_index = elem_head_offset;

        return cmd;
    }

    u32 write_matrices_and_materials(anton_stl::Slice<Matrix4 const> const matrices, anton_stl::Slice<Material const> const materials) {
        i64 material_head_offset = material_buffer.head - material_buffer.buffer;
        if (material_buffer.size - material_head_offset < materials.size()) {
            material_buffer.head = material_buffer.buffer;
            material_head_offset = 0;
        }
        memcpy(material_buffer.head, materials.data(), materials.size() * sizeof(Material));
        material_buffer.head += materials.size();

        i64 matrix_head_offset = matrix_buffer.head - matrix_buffer.buffer;
        if (matrix_buffer.size - matrix_head_offset < matrices.size()) {
            matrix_buffer.head = matrix_buffer.buffer;
            matrix_head_offset = 0;
        }
        memcpy(matrix_buffer.head, matrices.data(), matrices.size() * sizeof(Matrix4));
        matrix_buffer.head += matrices.size();

        return matrix_head_offset;
    }

    u64 write_persistent_geometry(anton_stl::Slice<Vertex const> const vertices, anton_stl::Slice<u32 const> const indices) {
        // TODO: Checks, safety, fencing, anything? Right now I'm pretty sure I'll not overwrite,
        //       but we'll need all of that in the future.
        i64 const index_offset = persistent_element_buffer.head - persistent_element_buffer.buffer;
        i64 const vertex_offset = persistent_vertex_buffer.head - persistent_vertex_buffer.buffer;
        if (index_offset + indices.size() > persistent_element_buffer.size || vertex_offset + vertices.size() > persistent_vertex_buffer.size) {
            throw std::runtime_error("Out of memory");
        }

        memcpy(persistent_vertex_buffer.head, vertices.data(), vertices.size() * sizeof(Vertex));
        persistent_vertex_buffer.head += vertices.size();
        memcpy(persistent_element_buffer.head, indices.data(), indices.size() * sizeof(u32));
        persistent_element_buffer.head += indices.size();
        Draw_Elements_Command cmd = {static_cast<u32>(indices.size()), 0, (u32)(element_buffer_index_count / 2 + index_offset),
                                     (u32)(vertex_buffer_vertex_count / 2 + vertex_offset), 0};
        u64 const handle = get_persistent_geometry_next_handle();
        persistent_draw_commands_map.emplace(handle, cmd);
        return handle;
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

    void add_draw_command(Draw_Elements_Command const command) {
        draw_elements_commands.push_back(command);
    }

    void add_draw_command(Draw_Persistent_Geometry_Command const cmd) {
        Draw_Elements_Command draw_cmd = persistent_draw_commands_map.at(cmd.handle);
        draw_cmd.base_instance = cmd.base_instance;
        draw_cmd.instance_count = cmd.instance_count;
        add_draw_command(draw_cmd);
    }

    void commit_draw() {
        if (draw_elements_commands.size() > 0) {
            if (draw_elements_cmd_buffer.head - draw_elements_cmd_buffer.buffer + draw_elements_commands.size() > draw_elements_cmd_buffer.size) {
                draw_elements_cmd_buffer.head = draw_elements_cmd_buffer.buffer;
            }

            memcpy(draw_elements_cmd_buffer.head, draw_elements_commands.data(), draw_elements_commands.size() * sizeof(Draw_Elements_Command));
            i64 const offset = (draw_elements_cmd_buffer.head - draw_elements_cmd_buffer.buffer) * sizeof(Draw_Elements_Command);
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)offset, draw_elements_commands.size(), sizeof(Draw_Elements_Command));
            draw_elements_cmd_buffer.head += draw_elements_commands.size();
            draw_elements_commands.clear();
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
        bind_vertex_buffers();
        auto objects = snapshot.view<Static_Mesh_Component, Transform>();
        Static_Mesh_Component last_mesh = {};
        Resource_Manager<Shader>& shader_manager = get_shader_manager();
        Resource_Manager<Material>& material_manager = get_material_manager();
        Resource_Manager<Mesh>& mesh_manager = get_mesh_manager();

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
        i64 current_draw = 0;
        Draw_Elements_Command cmd = {};
        // TODO: wrap around, write_geometry functions, etc.
        // Fairly dumb rendering loop.
        for (auto iter = objects.begin(), end = objects.end(); iter != end; ++iter) {
            auto const [transform, static_mesh] = objects.get<Transform, Static_Mesh_Component>(*iter);
            if (static_mesh.shader_handle != last_mesh.shader_handle || static_mesh.mesh_handle != last_mesh.mesh_handle ||
                static_mesh.material_handle != last_mesh.material_handle) {
                if (ANTON_LIKELY(current_draw != 0)) {
                    add_draw_command(cmd);
                }
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
                cmd = write_geometry(mesh.vertices, mesh.indices);
                cmd.instance_count = 1;
                cmd.base_instance = current_draw;
            }

            {
                // TODO: Does pointless calls to find_slot_and_bind_texture even though
                //       it doesn't actually rebind textures each iteration.
                //
                // Skip first texture slot since it should always have the default textures bound
                Material const mat = material_manager.get(static_mesh.material_handle);
                Material materials[1] = {mat};
                materials[0].diffuse_texture.index = find_slot_and_bind_texture(bound_textures, mat.diffuse_texture, current_draw);
                materials[0].specular_texture.index = find_slot_and_bind_texture(bound_textures, mat.specular_texture, current_draw);
                materials[0].normal_map.index = find_slot_and_bind_texture(bound_textures, mat.normal_map, current_draw);
                Matrix4 const matrices[1] = {to_matrix(transform)};
                [[maybe_unused]] u32 const base_instance = write_matrices_and_materials(matrices, materials);
                if (static_mesh.shader_handle != last_mesh.shader_handle || static_mesh.mesh_handle != last_mesh.mesh_handle ||
                    static_mesh.material_handle != last_mesh.material_handle) {
                    cmd.base_instance = base_instance;
                }
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
        framebuffer_info.depth_buffer.buffer_type = Framebuffer::Buffer_Type::texture;
        framebuffer_info.color_buffers.resize(2);
        // Normal
        framebuffer_info.color_buffers[0].internal_format = Framebuffer::Internal_Format::rgb32f;
        // albedo-specular
        framebuffer_info.color_buffers[1].internal_format = Framebuffer::Internal_Format::rgba8;
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

    void Renderer::render_frame(Matrix4 const view_mat, Matrix4 const projection_mat, Transform const camera_transform, Vector2 const viewport_size) {
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, viewport_size.x, viewport_size.y);
        bind_framebuffer(framebuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bind_mesh_vao();
        ECS& ecs = get_ecs();
        ECS snapshot = ecs.snapshot<Transform, Static_Mesh_Component>();
        render_scene(snapshot, camera_transform, view_mat, projection_mat);

        // Postprocessing

        glDisable(GL_DEPTH_TEST);
        bind_framebuffer(postprocess_back_buffer);
        glBindTextureUnit(0, framebuffer->get_depth_texture());
        glBindTextureUnit(1, framebuffer->get_color_texture(0));
        glBindTextureUnit(2, framebuffer->get_color_texture(1));

        Shader& deferred_shading = get_builtin_shader(Builtin_Shader::deferred_shading);
        deferred_shading.use();
        deferred_shading.set_vec3("camera.position", camera_transform.local_position);
        deferred_shading.set_vec2("viewport_size", viewport_size);
        deferred_shading.set_matrix4("inv_view_mat", to_matrix(camera_transform));
        deferred_shading.set_matrix4("inv_proj_mat", math::inverse(projection_mat));
        render_texture_quad();
        glEnable(GL_DEPTH_TEST);
        swap_postprocess_buffers();
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
