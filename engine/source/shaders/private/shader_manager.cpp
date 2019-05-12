#include "shader_manager.hpp"

#include "assets.hpp"
#include "debug_macros.hpp"
#include "engine.hpp"
#include "renderer.hpp"
#include "shader_exceptions.hpp"

Handle<Shader> Shader_Manager::add(Shader&& shader) {
    Handle<Shader> handle{shader.id.value()};
    shaders.emplace_back(std::move(shader));
    return handle;
}

Shader& Shader_Manager::get(Handle<Shader> const& handle) {
    for (Shader& shader: shaders) {
        if (shader.id.value() == handle.value) {
            return shader;
        }
    }

    throw std::invalid_argument("");
}

void Shader_Manager::remove(Handle<Shader> const& handle) {
    containers::Vector<Shader>::size_type i = 0;
    for (Shader& shader: shaders) {
        if (handle.value == shader.id.value()) {
            shaders.erase_unsorted_unchecked(i);
            break;
        }
        ++i;
    }
}

void Shader_Manager::reload_shaders() {
    try {
        Shader_File basic_vert = assets::load_shader_file("basicvertex.vert");
        Shader_File basic_frag = assets::load_shader_file("basicfrag.frag");
        Shader default_shader = create_shader(basic_vert, basic_frag);

        Shader_File unlit_vert = assets::load_shader_file("unlit_default.vert");
        Shader_File unlit_frag = assets::load_shader_file("unlit_default.frag");
        Shader unlit_default_shader = create_shader(unlit_vert, unlit_frag);

        swap(shaders.at(0), default_shader);
        swap(shaders.at(1), unlit_default_shader);
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