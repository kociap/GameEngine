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
#include "mesh/mesh_manager.hpp"
#include "mesh/plane.hpp"
#include "shader.hpp"
#include "shader_manager.hpp"
#include "window.hpp"
#include "math/transform.hpp"

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

		framebuffer_construct_info.width = shadow_width;
        framebuffer_construct_info.height = shadow_height;
        framebuffer_construct_info.depth_buffer_type = Framebuffer::Buffer_Type::texture;
        framebuffer_construct_info.color_buffer = false;
        light_depth_buffer = new Framebuffer(framebuffer_construct_info);

        // TODO move to postprocessing
        Assets::load_shader_file_and_attach(gamma_correction_shader, "postprocessing/postprocess_vertex.vert");
        Assets::load_shader_file_and_attach(gamma_correction_shader, "postprocessing/gamma_correction.frag");
        gamma_correction_shader.link();

        Assets::load_shader_file_and_attach(quad_shader, "quad.vert");
        Assets::load_shader_file_and_attach(quad_shader, "quad.frag");
        quad_shader.link();

		setup_opengl();
        set_gamma_value(gamma_correction_value);
    }

    Renderer::~Renderer() {
        delete framebuffer_multisampled;
        delete framebuffer;
        delete light_depth_buffer;
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
                Transform& transform = get_component<Transform>(directional_light.get_entity());
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].intensity", directional_light.intensity);
                shader.set_float("directional_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].position", transform.local_position);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].color", directional_light.color);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].direction", directional_light.direction);
                ++i;
            }
            i = 0;
            for (auto& spot_light : component_system.spot_light_components) {
                Transform& transform = get_component<Transform>(spot_light.get_entity());
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].intensity", spot_light.intensity);
                shader.set_float("spot_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].position", transform.local_position);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].color", spot_light.color);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].direction", spot_light.direction);
                shader.set_float("spot_lights[" + std::to_string(i) + "].cutoff_angle", spot_light.cutoff_angle);
                shader.set_float("spot_lights[" + std::to_string(i) + "].blend_angle", spot_light.blend_angle);
                ++i;
            }
            i = 0;
            for (auto& point_light : component_system.point_light_components) {
                Transform& transform = get_component<Transform>(point_light.get_entity());
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("point_lights[" + std::to_string(i) + "].intensity", point_light.intensity);
                shader.set_float("point_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("point_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("point_lights[" + std::to_string(i) + "].position", transform.local_position);
                shader.set_vec3("point_lights[" + std::to_string(i) + "].color", point_light.color);
                ++i;
            }

			shader.set_int("point_lights_count", component_system.point_light_components.size());
            shader.set_int("directional_lights_count", component_system.directional_light_components.size());

            // uhh?????????????????????
            shader.set_float("material.shininess", 32.0f);
        }
    }

    void Renderer::set_gamma_value(float gamma) {
        gamma_correction_value = gamma;
        gamma_correction_shader.set_float("gamma", 1 / gamma_correction_value);
	}

	void Renderer::setup_opengl() {
        glDisable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
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
            Transform& transform = get_component<Transform>(component.get_entity());
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
            Transform& transform = get_component<Transform>(component.get_entity());
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

		// Shadow mapping
        Component_System& component_system = Engine::get_component_system();
        Directional_Light_Component const& directional_light = component_system.directional_light_components.get(0);

        glViewport(0, 0, shadow_width, shadow_height);
        Framebuffer::bind(*light_depth_buffer);
        glClear(GL_DEPTH_BUFFER_BIT);
        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_NONE);
        Transform& dl_transform = get_component<Transform>(directional_light.get_entity());
		// TODO temporarily compute view transform of directional light. directional lights with direction up/down won't work properly
        Matrix4 dl_view_transform(transform::look_at(dl_transform.local_position, dl_transform.local_position + directional_light.direction, Vector3::up));
        Matrix4 dl_projection_transform(transform::orthographic(-10, 10, -10, 10, -100, 100));
        render_scene(dl_transform, dl_view_transform, dl_projection_transform);

        Window& window = Engine::get_window();

        glViewport(0, 0, window.width(), window.height());
        Framebuffer::bind(*framebuffer_multisampled);
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Camera& camera = find_active_camera();
        Transform& camera_transform = get_component<Transform>(camera.get_entity());
        Matrix4 view = camera.get_view_matrix();
        Matrix4 projection = camera.get_projection_matrix();
        render_scene(camera_transform, view, projection);

		Framebuffer::bind(*framebuffer_multisampled, Framebuffer::Bind_Mode::read);
        Framebuffer::bind(*framebuffer, Framebuffer::Bind_Mode::draw);
        framebuffer_multisampled->blit(*framebuffer, Framebuffer::Buffer_Mask::color);
        Framebuffer::bind_default();

        glDisable(GL_DEPTH_TEST);
        gamma_correction_shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer->get_depth_texture());
        gamma_correction_shader.set_int("scene_texture", 0);
        scene_quad.draw(gamma_correction_shader);
        glEnable(GL_DEPTH_TEST);
    }
} // namespace renderer
