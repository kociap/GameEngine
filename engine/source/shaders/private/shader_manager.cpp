#include "shader_manager.hpp"

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

Shader_Manager::iterator Shader_Manager::begin() {
    return shaders.begin();
}

Shader_Manager::iterator Shader_Manager::end() {
    return shaders.end();
}