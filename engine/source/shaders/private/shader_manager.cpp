#include "shader_manager.hpp"

#include "assets.hpp"
#include "shader_exceptions.hpp"
#include "debug_macros.hpp"
#include "engine.hpp"
#include "renderer.hpp"

Handle<Shader> Shader_Manager::add(Shader&& shader) {
    Handle<Shader> handle(shader.id.value());
    shaders.add(std::move(shader));
    return handle;
}

Shader& Shader_Manager::get(Handle<Shader> const& handle) {
    for (Shader& shader : shaders) {
        if (shader.id.value() == handle.value) {
            return shader;
        }
    }

    throw std::invalid_argument("");
}

void Shader_Manager::remove(Handle<Shader> const& handle) {
    Swapping_Pool<Shader>::size_t i = 0;
    for (Shader& shader : shaders) {
        if (handle.value == shader.id.value()) {
            shaders.remove_unchecked(i);
            break;
        }
        ++i;
    }
}

void Shader_Manager::reload_shaders() {
    try {
        Shader default_shader;
        Assets::load_shader_file_and_attach(default_shader, "basicvertex.vert");
        Assets::load_shader_file_and_attach(default_shader, "basicfrag.frag");
        default_shader.link();

        Shader unlit_default_shader;
        Assets::load_shader_file_and_attach(unlit_default_shader, "unlit_default.vert");
        Assets::load_shader_file_and_attach(unlit_default_shader, "unlit_default.frag");
        unlit_default_shader.link();

        Shader::swap_programs(shaders.get(0), default_shader);
        Shader::swap_programs(shaders.get(1), unlit_default_shader);
    } catch (Program_Linking_Failed& e) {
        GE_log("Failed to reload shaders due to linking error");
        GE_log(e.what());
    } catch (Shader_Compilation_Failed& e) {
        GE_log("Failed to reload shaders due to compilation error");
        GE_log(e.what());
    } catch (std::runtime_error& e) {
        GE_log("Failed to reload shaders due to unknown error");
        GE_log(e.what());
    }

	Engine::get_renderer().load_shader_light_properties();
}

Shader_Manager::iterator Shader_Manager::begin() {
    return shaders.begin();
}

Shader_Manager::iterator Shader_Manager::end() {
    return shaders.end();
}