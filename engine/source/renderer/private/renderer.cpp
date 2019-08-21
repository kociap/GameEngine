#include <renderer.hpp>
#include <renderer_internal.hpp>

#include <anton_stl/utility.hpp>
#include <assets.hpp>
#include <build_config.hpp>
#include <components/camera.hpp>
#include <components/directional_light_component.hpp>
#include <components/line_component.hpp>
#include <components/point_light_component.hpp>
#include <components/spot_light_component.hpp>
#include <components/static_mesh_component.hpp>
#include <components/transform.hpp>
#include <debug_macros.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>
#include <engine.hpp>
#include <framebuffer.hpp>
#include <glad.hpp>
#include <handle.hpp>
#include <math/matrix4.hpp>
#include <math/transform.hpp>
#include <mesh/mesh.hpp>
#include <mesh/plane.hpp>
#include <opengl.hpp>
#include <resource_manager.hpp>
#include <shader.hpp>

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

namespace rendering {
    Renderer::Renderer(int32_t width, int32_t height): single_color_shader(false), outline_mix_shader(false) {
        build_framebuffers(width, height);

        // TODO move to postprocessing
        auto postprocess_vert = assets::load_shader_file("postprocessing/postprocess_vertex.vert");
        auto gamma_correction = assets::load_shader_file("postprocessing/gamma_correction.frag");
        gamma_correction_shader = create_shader(postprocess_vert, gamma_correction);
        gamma_correction_shader.use();
        gamma_correction_shader.set_int("scene_texture", 0);

        auto outline_mix_file = assets::load_shader_file("editor/outline_mix.frag");
        outline_mix_shader = create_shader(outline_mix_file, postprocess_vert);
        outline_mix_shader.use();
        outline_mix_shader.set_int("scene_texture", 0);
        outline_mix_shader.set_int("outline_texture", 1);

        auto quad_vert = assets::load_shader_file("quad.vert");
        auto quad_frag = assets::load_shader_file("quad.frag");
        passthrough_quad_shader = create_shader(quad_vert, quad_frag);
        passthrough_quad_shader.use();
        passthrough_quad_shader.set_int("scene_texture", 0);

        auto deferred_frag = assets::load_shader_file("deferred_shading.frag");
        deferred_shading_shader = create_shader(deferred_frag, quad_vert);
        deferred_shading_shader.use();
        deferred_shading_shader.set_int("gbuffer_position", 0);
        deferred_shading_shader.set_int("gbuffer_normal", 1);
        deferred_shading_shader.set_int("gbuffer_albedo_spec", 2);

        auto tangents_vert = assets::load_shader_file("tangents.vert");
        auto tangents_geom = assets::load_shader_file("tangents.geom");
        auto tangents_frag = assets::load_shader_file("tangents.frag");
        tangents = create_shader(tangents_vert, tangents_geom, tangents_frag);

        auto single_color_vert = assets::load_shader_file("single_color.vert");
        auto single_color_frag = assets::load_shader_file("single_color.frag");
        single_color_shader = create_shader(single_color_frag, single_color_vert);

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

        setup_opengl();
        set_gamma_value(2.2f);
    }

    Renderer::~Renderer() {
        delete_framebuffers();
    }

    void Renderer::build_framebuffers(int32_t width, int32_t height) {
        Framebuffer::Construct_Info multisampled_framebuffer_info;
        multisampled_framebuffer_info.color_buffers.resize(1);
        multisampled_framebuffer_info.depth_buffer.enabled = true;
        multisampled_framebuffer_info.width = width;
        multisampled_framebuffer_info.height = height;
        multisampled_framebuffer_info.multisampled = true;
        multisampled_framebuffer_info.samples = 4;
        framebuffer_multisampled = new Framebuffer(multisampled_framebuffer_info);

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
        delete framebuffer_multisampled;
        delete framebuffer;
        //delete light_depth_buffer;
        delete postprocess_back_buffer;
        delete postprocess_front_buffer;
    }

    void Renderer::resize(int32_t width, int32_t height) {
        delete_framebuffers();
        build_framebuffers(width, height);
    }

    static void set_light_properties(Shader& shader, std::string const& index, Directional_Light_Component const& directional_light) {
        shader.set_float("directional_lights[" + index + "].intensity", directional_light.intensity);
        shader.set_float("directional_lights[" + index + "].diffuse_strength", 0.8f);
        shader.set_float("directional_lights[" + index + "].specular_strength", 1.0f);
        shader.set_vec3("directional_lights[" + index + "].color", directional_light.color);
        shader.set_vec3("directional_lights[" + index + "].direction", directional_light.direction);
    }

    static void set_light_properties(Shader& shader, std::string const& index, Transform& transform, Spot_Light_Component const& spot_light) {
        shader.set_float("spot_lights[" + index + "].attentuation_constant", 1.0f);
        shader.set_float("spot_lights[" + index + "].attentuation_linear", 0.09f);
        shader.set_float("spot_lights[" + index + "].attentuation_quadratic", 0.032f);
        shader.set_float("spot_lights[" + index + "].intensity", spot_light.intensity);
        shader.set_float("spot_lights[" + index + "].diffuse_strength", 0.8f);
        shader.set_float("spot_lights[" + index + "].specular_strength", 1.0f);
        shader.set_vec3("spot_lights[" + index + "].position", transform.local_position);
        shader.set_vec3("spot_lights[" + index + "].color", spot_light.color);
        shader.set_vec3("spot_lights[" + index + "].direction", spot_light.direction);
        shader.set_float("spot_lights[" + index + "].cutoff_angle", spot_light.cutoff_angle);
        shader.set_float("spot_lights[" + index + "].blend_angle", spot_light.blend_angle);
    }

    static void set_light_properties(Shader& shader, std::string const& index, Transform& transform, Point_Light_Component const& point_light) {
        shader.set_float("point_lights[" + index + "].attentuation_constant", 1.0f);
        shader.set_float("point_lights[" + index + "].attentuation_linear", 0.09f);
        shader.set_float("point_lights[" + index + "].attentuation_quadratic", 0.032f);
        shader.set_float("point_lights[" + index + "].intensity", point_light.intensity);
        shader.set_float("point_lights[" + index + "].diffuse_strength", 0.8f);
        shader.set_float("point_lights[" + index + "].specular_strength", 1.0f);
        shader.set_vec3("point_lights[" + index + "].position", transform.local_position);
        shader.set_vec3("point_lights[" + index + "].color", point_light.color);
    }

    static void set_shader_props(ECS& ecs, Shader& shader) {
        shader.use();
        shader.set_float("ambient_strength", 0.02f);
        shader.set_vec3("ambient_color", Color(1.0f, 1.0f, 1.0f));
        auto directional_lights = ecs.access<Directional_Light_Component>();
        auto spot_lights = ecs.access<Transform, Spot_Light_Component>();
        auto point_lights = ecs.access<Transform, Point_Light_Component>();
        uint32_t i = 0;
        for (Entity const entity: directional_lights) {
            set_light_properties(shader, std::to_string(i), directional_lights.get(entity));
            ++i;
        }
        i = 0;
        for (Entity const entity: spot_lights) {
            auto [transform, spot_light] = spot_lights.get<Transform, Spot_Light_Component>(entity);
            set_light_properties(shader, std::to_string(i), transform, spot_light);
            ++i;
        }
        i = 0;
        for (Entity const entity: point_lights) {
            auto [transform, point_light] = point_lights.get<Transform, Point_Light_Component>(entity);
            set_light_properties(shader, std::to_string(i), transform, point_light);
            ++i;
        }

        int32_t point_lights_count = static_cast<int32_t>(point_lights.size());
        shader.set_int("point_lights_count", point_lights_count);
        int32_t directional_lights_count = static_cast<int32_t>(directional_lights.size());
        shader.set_int("directional_lights_count", directional_lights_count);

        // uhh?????????????????????
        shader.set_float("material.shininess", 32.0f);
    }

    void Renderer::load_shader_light_properties() {
        ECS& ecs = get_ecs();
        Resource_Manager<Shader>& shader_manager = get_shader_manager();
        for (Shader& shader: shader_manager) {
            set_shader_props(ecs, shader);
        }

        set_shader_props(ecs, deferred_shading_shader);
    }

    void Renderer::update_dynamic_lights() {
        ECS& ecs = get_ecs();
        Resource_Manager<Shader>& shader_manager = get_shader_manager();
        for (Shader& shader: shader_manager) {
            set_shader_props(ecs, shader);
        }

        set_shader_props(ecs, deferred_shading_shader);
    }

    void Renderer::set_gamma_value(float gamma) {
        gamma_correction_shader.use();
        gamma_correction_shader.set_float("gamma", 1 / gamma);
    }

    void Renderer::setup_opengl() {
        glDisable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE); // TODO geometry shader stuff, remove
        CHECK_GL_ERRORS();
    }

    void Renderer::swap_postprocess_buffers() {
        anton_stl::swap(postprocess_front_buffer, postprocess_back_buffer);
    }

    void Renderer::render_shadow_map(Matrix4 const& view, Matrix4 const& projection) {
        Resource_Manager<Shader>& shader_manager = get_shader_manager();
        ECS& ecs = get_ecs();

        for (Shader& shader: shader_manager) {
            shader.use();
            shader.set_vec3("camera.position", Vector3());
            shader.set_matrix4("projection", projection);
            shader.set_matrix4("view", view);
        }

        auto static_meshes = ecs.access<Transform, Static_Mesh_Component>();
        for (Entity const entity: static_meshes) {
            auto [transform, static_mesh] = static_meshes.get<Transform, Static_Mesh_Component>(entity);
            Shader& shader = shader_manager.get(static_mesh.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(static_mesh, shader);
        }

        auto lines = ecs.access<Transform, Line_Component>();
        for (Entity const entity: lines) {
            auto [transform, line] = lines.get<Transform, Line_Component>(entity);
            Shader& shader = shader_manager.get(line.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(line, shader);
        }
    }

    void Renderer::render_scene(Transform const camera_transform, Matrix4 const view, Matrix4 const projection) {
        Resource_Manager<Shader>& shader_manager = get_shader_manager();
        ECS& ecs = get_ecs();
        //int32_t max_texture_count = opengl::get_max_combined_texture_units();

        //opengl::active_texture(max_texture_count - 1);
        //opengl::bind_texture(opengl::Texture_Type::texture_2D, light_depth_buffer->get_depth_texture());
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //Vector4 border_color(1, 1, 1, 1);
        //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border_color.x);
        for (Shader& shader: shader_manager) {
            shader.use();
            shader.set_vec3("camera.position", camera_transform.local_position);
            shader.set_matrix4("projection", projection);
            shader.set_matrix4("view", view);
            //shader.set_matrix4("light_space_transform", light_space_transform);
            //shader.set_int("shadow_map", max_texture_count - 1);
        }

        auto static_meshes = ecs.access<Transform, Static_Mesh_Component>();
        for (Entity const entity: static_meshes) {
            auto [transform, static_mesh] = static_meshes.get<Transform, Static_Mesh_Component>(entity);
            Shader& shader = shader_manager.get(static_mesh.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(static_mesh, shader);
        }

        auto lines = ecs.access<Transform, Line_Component>();
        for (Entity const entity: lines) {
            auto [transform, line] = lines.get<Transform, Line_Component>(entity);
            Shader& shader = shader_manager.get(line.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(line, shader);
        }
    }

    void Renderer::render_with_shader(Shader& shader, Transform const& camera_transform, Matrix4 const& view, Matrix4 const& projection) {
        ECS& ecs = get_ecs();

        shader.use();
        shader.set_vec3("camera.position", camera_transform.local_position);
        shader.set_matrix4("projection", projection);
        shader.set_matrix4("view", view);

        auto static_meshes = ecs.access<Transform, Static_Mesh_Component>();
        for (Entity const entity: static_meshes) {
            auto [transform, static_mesh] = static_meshes.get<Transform, Static_Mesh_Component>(entity);
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(static_mesh, shader);
        }

        auto lines = ecs.access<Transform, Line_Component>();
        for (Entity const entity: lines) {
            auto [transform, line] = lines.get<Transform, Line_Component>(entity);
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(line, shader);
        }
    }

    uint32_t Renderer::render_frame_as_texture(Matrix4 const view_mat, Matrix4 const proj_mat, Transform const camera_transform, int32_t const viewport_width,
                                               int32_t const viewport_height) {
        update_dynamic_lights();

        glEnable(GL_DEPTH_TEST);
        opengl::viewport(0, 0, viewport_width, viewport_height);
        Framebuffer::bind(*framebuffer);
        // glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
        opengl::clear_color(0.0f, 0.0f, 0.0f, 1.0f);
        // opengl::clear_color(1.0f, 1.0f, 1.0f, 1.0f);
        opengl::clear(opengl::Buffer_Mask::color_buffer_bit | opengl::Buffer_Mask::depth_buffer_bit);
        opengl::bind_vertex_array(mesh_vao);
        render_scene(camera_transform, view_mat, proj_mat);

        // Postprocessing

        glDisable(GL_DEPTH_TEST);
        Framebuffer::bind(*postprocess_back_buffer);
        opengl::active_texture(0);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, framebuffer->get_color_texture(0));
        opengl::active_texture(1);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, framebuffer->get_color_texture(1));
        opengl::active_texture(2);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, framebuffer->get_color_texture(2));
        deferred_shading_shader.use();
        deferred_shading_shader.set_vec3("camera.position", camera_transform.local_position);
        render_texture_quad();

        swap_postprocess_buffers();
        return postprocess_front_buffer->get_color_texture(0);
    }

    void Renderer::render_frame(Matrix4 const view_mat, Matrix4 const proj_mat, Transform const camera_transform, int32_t const viewport_width,
                                int32_t const viewport_height) {
        uint32_t frame_texture = render_frame_as_texture(view_mat, proj_mat, camera_transform, viewport_width, viewport_height);
        Framebuffer::bind_default();
        opengl::active_texture(0);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, frame_texture);
        gamma_correction_shader.use();
        render_texture_quad();
    }

    void render_texture_quad() {
        static Plane scene_quad;
        render_mesh(scene_quad);
    }

    void bind_material_properties(Material mat, Shader& shader) {
        shader.set_float("material.shininess", mat.shininess);
        opengl::active_texture(0);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, mat.diffuse_texture.handle);
        shader.set_int("material.texture_diffuse", 0);
        opengl::active_texture(1);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, mat.specular_texture.handle);
        shader.set_int("material.texture_specular", 1);
        opengl::active_texture(2);
        opengl::bind_texture(opengl::Texture_Type::texture_2D, mat.normal_map.handle);
        shader.set_int("material.normal_map", 2);
        shader.set_int("material.normal_map_attached", mat.normal_map.handle != 0);
    }

    void render_mesh(Mesh const& mesh) {
        glBindVertexBuffer(0, mesh.get_vbo(), 0, sizeof(Vertex));
        CHECK_GL_ERRORS();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.get_ebo());
        CHECK_GL_ERRORS();
        int32_t indices_count = static_cast<int32_t>(mesh.indices.size());
        opengl::draw_elements(GL_TRIANGLES, indices_count);
    }

    void render_object(Static_Mesh_Component const& component, Shader& shader) {
        auto& material_manager = get_material_manager();
        Material material = material_manager.get(component.material_handle);
        bind_material_properties(material, shader);
        Resource_Manager<Mesh>& mesh_manager = get_mesh_manager();
        Mesh& mesh = mesh_manager.get(component.mesh_handle);
        render_mesh(mesh);
    }

    void render_object(Line_Component const& component, Shader& shader) {
        auto& material_manager = get_material_manager();
        Material material = material_manager.get(component.material_handle);
        bind_material_properties(material, shader);
        Resource_Manager<Mesh>& mesh_manager = get_mesh_manager();
        Mesh& mesh = mesh_manager.get(component.mesh_handle);
        render_mesh(mesh);
    }
} // namespace rendering
