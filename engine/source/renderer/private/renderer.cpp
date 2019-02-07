#include "renderer.hpp"

#include "assets.hpp"
#include "components/camera.hpp"
#include "framebuffer.hpp"
#include "mesh/plane.hpp"
#include "window.hpp"

#include "components/component_system.hpp"
#include "engine.hpp"

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

    void Renderer::load_shaders() {
        Assets::load_shader_file_and_attach(shader, "basicvertex.vert");
        Assets::load_shader_file_and_attach(shader, "basicfrag.frag");
        shader.link();

        Assets::load_shader_file_and_attach(gamma_correction_shader, "postprocessing/postprocess_vertex.vert");
        Assets::load_shader_file_and_attach(gamma_correction_shader, "postprocessing/gamma_correction.frag");
        gamma_correction_shader.link();

        Assets::load_shader_file_and_attach(quad_shader, "quad.vert");
        Assets::load_shader_file_and_attach(quad_shader, "quad.frag");
        quad_shader.link();

        load_shaders_properties();
    }

    void Renderer::load_shaders_properties() {
        auto set_shader_light_properties = [](Shader& shader, Component_System& component_sys) {
            shader.use();
            shader.set_float("ambient_strength", 0.02f);
            shader.set_vec3("ambient_color", Color(1.0f, 1.0f, 1.0f));
            int i = 0;
            for (auto& directional_light : component_sys.directional_light_components) {
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].intensity", 6.0f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("directional_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].position", directional_light.transform.position);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].color", directional_light.color);
                shader.set_vec3("directional_lights[" + std::to_string(i) + "].direction", directional_light.direction);
                ++i;
            }
            i = 0;
            for (auto& spot_light : component_sys.spot_light_components) {
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].intensity", 6.0f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("spot_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].position", spot_light.transform.position);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].color", spot_light.color);
                shader.set_vec3("spot_lights[" + std::to_string(i) + "].direction", spot_light.direction);
                shader.set_float("spot_lights[" + std::to_string(i) + "].cutoff_angle", spot_light.cutoff_angle);
                shader.set_float("spot_lights[" + std::to_string(i) + "].blend_angle", spot_light.blend_angle);
                ++i;
            }
            i = 0;
            for (auto& point_light : component_sys.point_light_components) {
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_constant", 1.0f);
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_linear", 0.09f);
                shader.set_float("point_lights[" + std::to_string(i) + "].attentuation_quadratic", 0.032f);
                shader.set_float("point_lights[" + std::to_string(i) + "].intensity", 6.0f);
                shader.set_float("point_lights[" + std::to_string(i) + "].diffuse_strength", 0.8f);
                shader.set_float("point_lights[" + std::to_string(i) + "].specular_strength", 1.0f);
                shader.set_vec3("point_lights[" + std::to_string(i) + "].position", point_light.transform.position);
                shader.set_vec3("point_lights[" + std::to_string(i) + "].color", point_light.color);
                ++i;
            }
        };

        Component_System& component_system = Engine::get_component_system();
        set_shader_light_properties(shader, component_system);

        // uhh?????????????????????
        shader.set_float("material.shininess", 32.0f);
    }

    void Renderer::render_scene() {
        /*Matrix4 view = Camera::main->get_view_transform();
        Matrix4 projection = Camera::main->get_projection_transform();*/
    }

    void Renderer::render_frame() {
        static Plane scene_quad;

        glDisable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // glViewport(0, 0, shadow_width, shadow_height);
        // light_depth_buffer.bind();
        // glClear(GL_DEPTH_BUFFER_BIT);
        // render_scene();

        glEnable(GL_FRAMEBUFFER_SRGB);

		Window& window = Engine::get_window();

        glViewport(0, 0, window.width(), window.height());
        framebuffer_multisampled->bind();
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_scene();

        framebuffer_multisampled->blit(*framebuffer);
        glDisable(GL_DEPTH_TEST);
        gamma_correction_shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer->get_texture());
        gamma_correction_shader.set_int("scene_texture", 0);
        gamma_correction_shader.set_float("gamma", 1 / gamma_correction_value);
        scene_quad.draw(gamma_correction_shader);
    }

    void Renderer::register_component(Renderable_Component* component) {
        registered_components.add(component);
    }

    void Renderer::unregister_component(Renderable_Component* component) {
        Swapping_Pool<Renderable_Component*>::size_t i = 0;
        for (auto ptr : registered_components) {
            if (ptr == component) {
                registered_components.remove(i);
                break;
            }
            ++i;
        }
    }
} // namespace renderer
