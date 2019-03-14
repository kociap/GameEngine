#include "renderer.hpp"

#include "assets.hpp"
#include "components/camera.hpp"
#include "components/component_system.hpp"
#include "components/transform.hpp"
#include "debug_macros.hpp"
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
        Framebuffer::Construct_Info framebuffer_construct_info;
        framebuffer_construct_info.color_buffers[0].enabled = true;
        framebuffer_construct_info.depth_buffer.enabled = true;
        framebuffer_construct_info.width = window.width();
        framebuffer_construct_info.height = window.height();
        framebuffer_construct_info.multisampled = true;
        framebuffer_construct_info.samples = 4;

        framebuffer_multisampled = new Framebuffer(framebuffer_construct_info);

        framebuffer_construct_info.multisampled = false;
        framebuffer = new Framebuffer(framebuffer_construct_info);

        //framebuffer_construct_info.width = shadow_width;
        //framebuffer_construct_info.height = shadow_height;
        //framebuffer_construct_info.depth_buffer_type = Framebuffer::Buffer_Type::texture;
        //framebuffer_construct_info.color_buffer = false;
        //light_depth_buffer = new Framebuffer(framebuffer_construct_info);

        Framebuffer::Construct_Info postprocess_construct_info;
        postprocess_construct_info.color_buffers[0].enabled = true;
        postprocess_construct_info.width = window.width();
        postprocess_construct_info.height = window.height();

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

    static void set_light_properties(Shader& shader, std::string const& index, Spot_Light_Component const& spot_light) {
        Transform& transform = get_component<Transform>(spot_light.get_entity());
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

    static void set_light_properties(Shader& shader, std::string const& index, Point_Light_Component const& point_light) {
        Transform& transform = get_component<Transform>(point_light.get_entity());
        shader.set_float("point_lights[" + index + "].attentuation_constant", 1.0f);
        shader.set_float("point_lights[" + index + "].attentuation_linear", 0.09f);
        shader.set_float("point_lights[" + index + "].attentuation_quadratic", 0.032f);
        shader.set_float("point_lights[" + index + "].intensity", point_light.intensity);
        shader.set_float("point_lights[" + index + "].diffuse_strength", 0.8f);
        shader.set_float("point_lights[" + index + "].specular_strength", 1.0f);
        shader.set_vec3("point_lights[" + index + "].position", transform.local_position);
        shader.set_vec3("point_lights[" + index + "].color", point_light.color);
    }

    void Renderer::load_shader_light_properties() {
        Component_System& component_system = Engine::get_component_system();
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        for (Shader& shader : shader_manager) {
            shader.use();
            shader.set_float("ambient_strength", 0.02f);
            shader.set_vec3("ambient_color", Color(1.0f, 1.0f, 1.0f));
            uint32_t i = 0;
            for (auto& directional_light : component_system.directional_light_components) {
                set_light_properties(shader, std::to_string(i), directional_light);
                ++i;
            }
            i = 0;
            for (auto& spot_light : component_system.spot_light_components) {
                set_light_properties(shader, std::to_string(i), spot_light);
                ++i;
            }
            i = 0;
            for (auto& point_light : component_system.point_light_components) {
                set_light_properties(shader, std::to_string(i), point_light);
                ++i;
            }

            shader.set_int("point_lights_count", component_system.point_light_components.size());
            shader.set_int("directional_lights_count", component_system.directional_light_components.size());

            // uhh?????????????????????
            shader.set_float("material.shininess", 32.0f);
        }
    }

    void Renderer::update_dynamic_lights() {
        Component_System& component_system = Engine::get_component_system();
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        for (Shader& shader : shader_manager) {
            shader.use();
            shader.set_float("ambient_strength", 0.02f);
            shader.set_vec3("ambient_color", Color(1.0f, 1.0f, 1.0f));
            uint32_t i = 0;
            for (auto& directional_light : component_system.directional_light_components) {
                if (directional_light.dynamic) {
                    set_light_properties(shader, std::to_string(i), directional_light);
                }
                ++i;
            }
            i = 0;
            for (auto& spot_light : component_system.spot_light_components) {
                if (spot_light.dynamic) {
                    set_light_properties(shader, std::to_string(i), spot_light);
                }
                ++i;
            }
            i = 0;
            for (auto& point_light : component_system.point_light_components) {
                if (point_light.dynamic) {
                    set_light_properties(shader, std::to_string(i), point_light);
                }
                ++i;
            }
        }
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

    Camera& Renderer::find_active_camera() {
        Component_System& component_system = Engine::get_component_system();
        for (Camera& camera : component_system.camera_components) {
            if (camera.active) {
                return camera;
            }
        }

        throw std::runtime_error("No active camera found"); // TODO ???
    }

    void Renderer::render_mesh(Mesh& mesh, Shader& shader) {
        CHECK_GL_ERRORS();
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
        Component_System& component_system = Engine::get_component_system();

        for (Shader& shader : shader_manager) {
            shader.use();
            shader.set_vec3("camera.position", Vector3());
            shader.set_matrix4("projection", projection);
            shader.set_matrix4("view", view);
        }

        for (Static_Mesh_Component& component : component_system.static_mesh_components) {
            Shader& shader = shader_manager.get(component.shader_handle);
            shader.use();
            Transform& transform = get_component<Transform>(component.get_entity());
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(component, shader);
        }

        for (Line_Component& component : component_system.line_components) {
            Shader& shader = shader_manager.get(component.shader_handle);
            shader.use();
            Transform& transform = get_component<Transform>(component.get_entity());
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(component, shader);
        }
    }

    void Renderer::render_scene(Transform const& camera_transform, Matrix4 const& view, Matrix4 const& projection, Matrix4 const& light_space_transform) {
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        Component_System& component_system = Engine::get_component_system();
        int32_t max_texture_count = opengl::get_max_combined_texture_units();

        //opengl::active_texture(max_texture_count - 1);
        //opengl::bind_texture(GL_TEXTURE_2D, light_depth_buffer->get_depth_texture());
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //Vector4 border_color(1, 1, 1, 1);
        //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border_color.x);
        for (Shader& shader : shader_manager) {
            shader.use();
            shader.set_vec3("camera.position", camera_transform.local_position);
            shader.set_matrix4("projection", projection);
            shader.set_matrix4("view", view);
            //shader.set_matrix4("light_space_transform", light_space_transform);
            //shader.set_int("shadow_map", max_texture_count - 1);
        }

        for (Static_Mesh_Component& component : component_system.static_mesh_components) {
            Shader& shader = shader_manager.get(component.shader_handle);
            shader.use();
            Transform& transform = get_component<Transform>(component.get_entity());
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(component, shader);
        }

        for (Line_Component& component : component_system.line_components) {
            Shader& shader = shader_manager.get(component.shader_handle);
            shader.use();
            Transform& transform = get_component<Transform>(component.get_entity());
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(component, shader);
        }

        // Unbind just in case
        opengl::bind_vertex_array(0);
    }

    void Renderer::render_with_shader(Shader& shader, Transform const& camera_transform, Matrix4 const& view, Matrix4 const& projection) {
        Component_System& component_system = Engine::get_component_system();

        shader.use();
        shader.set_vec3("camera.position", camera_transform.local_position);
        shader.set_matrix4("projection", projection);
        shader.set_matrix4("view", view);

        for (Static_Mesh_Component& component : component_system.static_mesh_components) {
            Transform& transform = get_component<Transform>(component.get_entity());
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(component, shader);
        }

        for (Line_Component& component : component_system.line_components) {
            Transform& transform = get_component<Transform>(component.get_entity());
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            render_object(component, shader);
        }
    }

    void Renderer::render_frame() {
        static Plane scene_quad;

        update_dynamic_lights();

        // Shadow mapping
        Component_System& component_system = Engine::get_component_system();
        Directional_Light_Component const& directional_light = component_system.directional_light_components.get(0);

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
        glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
        //glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Camera& camera = find_active_camera();
        Transform& camera_transform = get_component<Transform>(camera.get_entity());
        Matrix4 view = camera.get_view_matrix();
        Matrix4 projection = camera.get_projection_matrix();
        render_scene(camera_transform, view, projection, dl_view_transform * dl_projection_transform);

        // Render normals and tangents
        //render_with_shader(tangents, camera_transform, view, projection);

        // Framebuffer::bind(*framebuffer_multisampled, Framebuffer::Bind_Mode::read);
        // Framebuffer::bind(*framebuffer, Framebuffer::Bind_Mode::draw);
        // framebuffer_multisampled->blit(*framebuffer, Framebuffer::Buffer_Mask::color);

        glDisable(GL_DEPTH_TEST);
        // Framebuffer::bind(*postprocess_back_buffer);
        opengl::active_texture(0);

        // swap_postprocess_buffers();
        Framebuffer::bind_default();
        if (output_shadow_map) {
            opengl::bind_texture(GL_TEXTURE_2D, light_depth_buffer->get_depth_texture());
            quad_shader.use();
            render_mesh(scene_quad, quad_shader);
        } else {
            opengl::bind_texture(GL_TEXTURE_2D, framebuffer->get_color_texture(0));
            gamma_correction_shader.use();
            render_mesh(scene_quad, gamma_correction_shader);
        }

        glEnable(GL_DEPTH_TEST);
    }
} // namespace renderer
