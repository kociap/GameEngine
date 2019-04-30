#include "renderer.hpp"

#include "assets.hpp"
#include "components/camera.hpp"
#include "components/directional_light_component.hpp"
#include "components/line_component.hpp"
#include "components/point_light_component.hpp"
#include "components/spot_light_component.hpp"
#include "components/static_mesh_component.hpp"
#include "components/transform.hpp"
#include "debug_macros.hpp"
#include "ecs/ecs.hpp"
#include "engine.hpp"
#include "framebuffer.hpp"
#include "handle.hpp"
#include "math/matrix4.hpp"
#include "math/transform.hpp"
#include "mesh/mesh_manager.hpp"
#include "mesh/plane.hpp"
#include "opengl.hpp"
#include "shader.hpp"
#include "shader_manager.hpp"
#include "window.hpp"

namespace renderer {
    Renderer::Renderer() {
        opengl::load_opengl_parameters();

        Window& window = Engine::get_window();
        uint32_t window_height = window.height();
        uint32_t window_width = window.width();

        Framebuffer::Construct_Info multisampled_framebuffer_info;
        multisampled_framebuffer_info.color_buffers.resize(1);
        multisampled_framebuffer_info.depth_buffer.enabled = true;
        multisampled_framebuffer_info.width = window_width;
        multisampled_framebuffer_info.height = window_height;
        multisampled_framebuffer_info.multisampled = true;
        multisampled_framebuffer_info.samples = 4;
        framebuffer_multisampled = new Framebuffer(multisampled_framebuffer_info);

        Framebuffer::Construct_Info framebuffer_info;
        framebuffer_info.width = window_width;
        framebuffer_info.height = window_height;
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
        postprocess_construct_info.width = window_width;
        postprocess_construct_info.height = window_height;
        postprocess_back_buffer = new Framebuffer(postprocess_construct_info);
        postprocess_front_buffer = new Framebuffer(postprocess_construct_info);

        // TODO move to postprocessing
        Assets::load_shader_file_and_attach(gamma_correction_shader, "postprocessing/postprocess_vertex.vert");
        Assets::load_shader_file_and_attach(gamma_correction_shader, "postprocessing/gamma_correction.frag");
        gamma_correction_shader.link();
        gamma_correction_shader.use();
        gamma_correction_shader.set_int("scene_texture", 0);

        Assets::load_shader_file_and_attach(quad_shader, "quad.vert");
        Assets::load_shader_file_and_attach(quad_shader, "quad.frag");
        quad_shader.link();
        quad_shader.use();
        quad_shader.set_int("scene_texture", 0);

        Assets::load_shader_file_and_attach(deferred_shading_shader, "deferred_shading.frag");
        Assets::load_shader_file_and_attach(deferred_shading_shader, "quad.vert");
        deferred_shading_shader.link();
        deferred_shading_shader.use();
        deferred_shading_shader.set_int("gbuffer_position", 0);
        deferred_shading_shader.set_int("gbuffer_normal", 1);
        deferred_shading_shader.set_int("gbuffer_albedo_spec", 2);

        Assets::load_shader_file_and_attach(tangents, "tangents.vert");
        Assets::load_shader_file_and_attach(tangents, "tangents.geom");
        Assets::load_shader_file_and_attach(tangents, "tangents.frag");
        tangents.link();

        setup_opengl();
        set_gamma_value(gamma_correction_value);
    }

    Renderer::~Renderer() {
        delete framebuffer_multisampled;
        delete framebuffer;
        //delete light_depth_buffer;
        delete postprocess_back_buffer;
        delete postprocess_front_buffer;
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
            auto& [transform, spot_light] = spot_lights.get<Transform, Spot_Light_Component>(entity);
            set_light_properties(shader, std::to_string(i), transform, spot_light);
            ++i;
        }
        i = 0;
        for (Entity const entity: point_lights) {
            auto& [transform, point_light] = point_lights.get<Transform, Point_Light_Component>(entity);
            set_light_properties(shader, std::to_string(i), transform, point_light);
            ++i;
        }

        shader.set_int("point_lights_count", point_lights.size());
        shader.set_int("directional_lights_count", directional_lights.size());

        // uhh?????????????????????
        shader.set_float("material.shininess", 32.0f);
    }

    void Renderer::load_shader_light_properties() {
        ECS& ecs = Engine::get_ecs();
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        for (Shader& shader: shader_manager) {
            set_shader_props(ecs, shader);
        }

        set_shader_props(ecs, deferred_shading_shader);
    }

    void Renderer::update_dynamic_lights() {
        ECS& ecs = Engine::get_ecs();
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        for (Shader& shader: shader_manager) {
            set_shader_props(ecs, shader);
        }

        set_shader_props(ecs, deferred_shading_shader);
    }

    void Renderer::set_gamma_value(float gamma) {
        gamma_correction_value = gamma;
        gamma_correction_shader.use();
        gamma_correction_shader.set_float("gamma", 1 / gamma_correction_value);
    }

    void Renderer::setup_opengl() {
        glDisable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_PROGRAM_POINT_SIZE); // TODO geometry shader stuff, remove
        CHECK_GL_ERRORS();
    }

    void Renderer::swap_postprocess_buffers() {
        std::swap(postprocess_front_buffer, postprocess_back_buffer);
    }

    void Renderer::render_mesh(Mesh& mesh, Shader& shader) {
        shader.set_int("material.normal_map_attached", 0);
        uint32_t specular = 0;
        uint32_t diffuse = 0;
        for (uint32_t i = 0; i < mesh.textures.size(); ++i) {
            opengl::active_texture(i);
            if (mesh.textures[i].type == Texture_Type::diffuse) {
                shader.set_int("material.texture_diffuse" + std::to_string(diffuse), i);
                ++diffuse;
            } else if (mesh.textures[i].type == Texture_Type::specular) {
                shader.set_int("material.texture_specular" + std::to_string(specular), i);
                ++specular;
            } else {
                shader.set_int("material.normal_map", i);
                shader.set_int("material.normal_map_attached", 1);
            }
            opengl::bind_texture(GL_TEXTURE_2D, mesh.textures[i].id);
        }
        uint32_t vao = mesh.get_vao();
        opengl::bind_vertex_array(vao);
        opengl::draw_elements(GL_TRIANGLES, mesh.indices.size());
    }

    void Renderer::render_mesh_instanced(Mesh& mesh, Shader& shader, uint32_t count) {
        for (uint32_t i = 0; i < mesh.textures.size(); ++i) {
            opengl::active_texture(i);
            if (mesh.textures[i].type == Texture_Type::diffuse) {
                shader.set_int("material.texture_diffuse", i);
            } else if (mesh.textures[i].type == Texture_Type::specular) {
                shader.set_int("material.texture_specular", i);
            } else {
                shader.set_int("material.normal_map", i);
                shader.set_int("material.normal_map_attached", 1);
            }
            opengl::bind_texture(GL_TEXTURE_2D, mesh.textures[i].id);
        }

        uint32_t vao = mesh.get_vao();
        opengl::bind_vertex_array(vao);
        opengl::draw_elements_instanced(GL_TRIANGLES, mesh.indices.size(), count);
    }

    void Renderer::render_object(Static_Mesh_Component const& component, Shader& shader) {
        Mesh_Manager& mesh_manager = Engine::get_mesh_manager();
        Mesh& mesh = mesh_manager.get(component.mesh_handle);
        render_mesh(mesh, shader);
    }

    void Renderer::render_object(Line_Component const& component, Shader& shader) {
        Mesh_Manager& mesh_manager = Engine::get_mesh_manager();
        Mesh& mesh = mesh_manager.get(component.mesh_handle);
        render_mesh(mesh, shader);
    }

    void Renderer::render_shadow_map(Matrix4 const& view, Matrix4 const& projection) {
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        ECS& ecs = Engine::get_ecs();

        for (Shader& shader: shader_manager) {
            shader.use();
            shader.set_vec3("camera.position", Vector3());
            shader.set_matrix4("projection", projection);
            shader.set_matrix4("view", view);
        }

        auto static_meshes = ecs.access<Transform, Static_Mesh_Component>();
        for (Entity const entity: static_meshes) {
            auto& [transform, static_mesh] = static_meshes.get<Transform, Static_Mesh_Component>(entity);
            Shader& shader = shader_manager.get(static_mesh.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(static_mesh, shader);
        }

        auto lines = ecs.access<Transform, Line_Component>();
        for (Entity const entity: lines) {
            auto& [transform, line] = lines.get<Transform, Line_Component>(entity);
            Shader& shader = shader_manager.get(line.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(line, shader);
        }
    }

    void Renderer::render_scene(Transform const& camera_transform, Matrix4 const& view, Matrix4 const& projection, Matrix4 const& light_space_transform) {
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        ECS& ecs = Engine::get_ecs();
        int32_t max_texture_count = opengl::get_max_combined_texture_units();

        //opengl::active_texture(max_texture_count - 1);
        //opengl::bind_texture(GL_TEXTURE_2D, light_depth_buffer->get_depth_texture());
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
            auto& [transform, static_mesh] = static_meshes.get<Transform, Static_Mesh_Component>(entity);
            Shader& shader = shader_manager.get(static_mesh.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(static_mesh, shader);
        }

        auto lines = ecs.access<Transform, Line_Component>();
        for (Entity const entity: lines) {
            auto& [transform, line] = lines.get<Transform, Line_Component>(entity);
            Shader& shader = shader_manager.get(line.shader_handle);
            shader.use();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(line, shader);
        }

        // Unbind just in case
        opengl::bind_vertex_array(0);
    }

    void Renderer::render_with_shader(Shader& shader, Transform const& camera_transform, Matrix4 const& view, Matrix4 const& projection) {
        ECS& ecs = Engine::get_ecs();

        shader.use();
        shader.set_vec3("camera.position", camera_transform.local_position);
        shader.set_matrix4("projection", projection);
        shader.set_matrix4("view", view);

        auto static_meshes = ecs.access<Transform, Static_Mesh_Component>();
        for (Entity const entity: static_meshes) {
            auto& [transform, static_mesh] = static_meshes.get<Transform, Static_Mesh_Component>(entity);
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(static_mesh, shader);
        }

        auto lines = ecs.access<Transform, Line_Component>();
        for (Entity const entity: lines) {
            auto& [transform, line] = lines.get<Transform, Line_Component>(entity);
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(line, shader);
        }
    }

    static std::pair<Camera&, Transform&> find_active_camera() {
        ECS& ecs = Engine::get_ecs();
        auto cameras = ecs.access<Camera, Transform>();
        for (Entity const entity: cameras) {
            Camera& camera = cameras.get<Camera>(entity);
            if (camera.active) {
                return {camera, cameras.get<Transform>(entity)};
            }
        }

        throw std::runtime_error("No active camera found"); // TODO ???
    }

    void Renderer::render_frame() {
        static Plane scene_quad;

        update_dynamic_lights();

        // Shadow mapping
        ECS& ecs = Engine::get_ecs();
        Directional_Light_Component const& directional_light = ecs.raw<Directional_Light_Component>()[0];

        //glViewport(0, 0, shadow_width, shadow_height);
        //Framebuffer::bind(*light_depth_buffer);
        //glClear(GL_DEPTH_BUFFER_BIT);
        // TODO temporarily compute view transform of directional light. directional lights with direction up/down won't work properly
        Matrix4 dl_view_transform(transform::look_at(Vector3(), directional_light.direction, Vector3::up));
        Matrix4 dl_projection_transform(transform::orthographic(-10, 10, -10, 10, -100, 100));
        //render_shadow_map(dl_view_transform, dl_projection_transform);

        Window& window = Engine::get_window();

        glViewport(0, 0, window.width(), window.height());
        // Framebuffer::bind(*framebuffer_multisampled);
        Framebuffer::bind(*framebuffer);
        //glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto& [camera, camera_transform] = find_active_camera();
        Matrix4 view = Camera::get_view_matrix(camera_transform);
        Matrix4 projection = Camera::get_projection_matrix(camera, camera_transform);
        render_scene(camera_transform, view, projection, dl_view_transform * dl_projection_transform);

        glDisable(GL_DEPTH_TEST);
        Framebuffer::bind(*postprocess_back_buffer);

        opengl::active_texture(0);
        opengl::bind_texture(GL_TEXTURE_2D, framebuffer->get_color_texture(0));
        opengl::active_texture(1);
        opengl::bind_texture(GL_TEXTURE_2D, framebuffer->get_color_texture(1));
        opengl::active_texture(2);
        opengl::bind_texture(GL_TEXTURE_2D, framebuffer->get_color_texture(2));
        deferred_shading_shader.use();
        deferred_shading_shader.set_vec3("camera.position", camera_transform.local_position);
        render_mesh(scene_quad, deferred_shading_shader);

        swap_postprocess_buffers();
        Framebuffer::bind_default();
        opengl::active_texture(0);
        if (output_shadow_map) {
            opengl::bind_texture(GL_TEXTURE_2D, postprocess_front_buffer->get_depth_texture());
            quad_shader.use();
            render_mesh(scene_quad, quad_shader);
        } else {
            opengl::bind_texture(GL_TEXTURE_2D, postprocess_front_buffer->get_color_texture(0));
            gamma_correction_shader.use();
            render_mesh(scene_quad, gamma_correction_shader);
        }

        glEnable(GL_DEPTH_TEST);
    }
} // namespace renderer
