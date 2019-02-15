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
#include "mesh/plane.hpp"
#include "mesh/mesh_manager.hpp"
#include "shader.hpp"
#include "shader_manager.hpp"
#include "window.hpp"

namespace renderer {
    Renderer::Renderer() {
        Window& window = Engine::get_window();
        Framebuffer::Construct_Info framebuffer_construct_info;
        framebuffer_construct_info.width = window.width();
        framebuffer_construct_info.height = window.height();
        framebuffer_construct_info.depth_buffer = true;
        framebuffer_construct_info.multisampled = true;
        framebuffer_construct_info.samples = 4;

        framebuffer_multisampled = new Framebuffer(framebuffer_construct_info);

        framebuffer_construct_info.multisampled = false;
        framebuffer = new Framebuffer(framebuffer_construct_info);

        framebuffer_construct_info.depth_buffer_type = Framebuffer::Buffer_Type::texture;
        framebuffer_construct_info.color_buffer = false;
        light_depth_buffer = new Framebuffer(framebuffer_construct_info);
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
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].intensity", directional_light.intensity);
                shader.set_float("directional_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].position", directional_light.get_transform().local_position);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].color", directional_light.color);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].direction", directional_light.direction);
                ++i;
            }
            i = 0;
            for (auto& spot_light : component_system.spot_light_components) {
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].intensity", spot_light.intensity);
                shader.set_float("spot_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].position", spot_light.get_transform().local_position);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].color", spot_light.color);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].direction", spot_light.direction);
                shader.set_float("spot_lights[" + std::to_string(i) + "].cutoff_angle", spot_light.cutoff_angle);
                shader.set_float("spot_lights[" + std::to_string(i) + "].blend_angle", spot_light.blend_angle);
                ++i;
            }
            i = 0;
            for (auto& point_light : component_system.point_light_components) {
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("point_lights[" + std::to_string(i) + "].intensity", point_light.intensity);
                shader.set_float("point_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("point_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("point_lights[" + std::to_string(i) + "].position", point_light.get_transform().local_position);
                shader.set_vec3("point_lights[" + std::to_string(i) + "].color", point_light.color);
                ++i;
            }

            // uhh?????????????????????
            shader.set_float("material.shininess", 32.0f);
        }
    }

    Camera& Renderer::find_active_camera() {
        Component_System& component_system = Engine::get_component_system();
        for (Camera& camera : component_system.camera_components) {
            if (camera.is_active()) {
                return camera;
            }
        }

        throw std::runtime_error("No active camera found");
    }

    void Renderer::render_object(Static_Mesh_Component const& component, Shader& shader) {
        Mesh_Manager& mesh_manager = Engine::get_mesh_manager();
        Mesh& mesh = mesh_manager.get(component.mesh_handle);
        mesh.draw(shader);
    }

    void Renderer::render_object(Line_Component const& component, Shader& shader) {
        Mesh_Manager& mesh_manager = Engine::get_mesh_manager();
        Mesh& mesh = mesh_manager.get(component.mesh_handle);
        mesh.draw(shader);
    }

    void Renderer::render_scene(Transform const& camera_transform, Matrix4 const& view, Matrix4 const& projection) {
        Shader_Manager& shader_manager = Engine::get_shader_manager();
        Component_System& component_system = Engine::get_component_system();
        for (Static_Mesh_Component& component : component_system.static_mesh_components) {
            Shader& shader = shader_manager.get(component.shader_handle);
            shader.use();
            Transform& transform = component.game_object.get_component<Transform>();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            shader.set_vec3("camera.position", camera_transform.local_position);
            shader.set_matrix4("projection", projection);
            shader.set_matrix4("view", view);
            render_object(component, shader);
        }

        for (Line_Component& component : component_system.line_components) {
            Shader& shader = shader_manager.get(component.shader_handle);
            shader.use();
            Transform& transform = component.game_object.get_component<Transform>();
            Matrix4 model(transform.to_matrix());
            shader.set_matrix4("model", model);
            shader.set_vec3("camera.position", camera_transform.local_position);
            shader.set_matrix4("projection", projection);
            shader.set_matrix4("view", view);
            render_object(component, shader);
        }
    }

    void Renderer::render_frame() {
        static Plane scene_quad;

        glDisable(GL_FRAMEBUFFER_SRGB);
        //glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // glViewport(0, 0, shadow_width, shadow_height);
        // light_depth_buffer.bind();
        // glClear(GL_DEPTH_BUFFER_BIT);
        // render_scene();

        glEnable(GL_FRAMEBUFFER_SRGB);

        Window& window = Engine::get_window();

        glViewport(0, 0, window.width(), window.height());
        //framebuffer_multisampled->bind();
        //glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Camera& camera = find_active_camera();
        Transform& camera_transform = camera.get_transform();
        Matrix4 view = camera.get_view_transform();
        Matrix4 projection = camera.get_projection_transform();
        render_scene(camera_transform, view, projection);

        /*framebuffer_multisampled->blit(*framebuffer);
        glDisable(GL_DEPTH_TEST);
        gamma_correction_shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer->get_texture());
        gamma_correction_shader.set_int("scene_texture", 0);
        gamma_correction_shader.set_float("gamma", 1 / gamma_correction_value);
        scene_quad.draw(gamma_correction_shader);*/
    }
} // namespace renderer
