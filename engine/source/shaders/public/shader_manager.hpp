#ifndef SHADERS_SHADER_MANAGER_HPP_INCLUDE
#define SHADERS_SHADER_MANAGER_HPP_INCLUDE

#include "containers/swapping_pool.hpp"
#include "handle.hpp"
#include "shader.hpp"

class Shader_Manager {
public:
    using iterator = Swapping_Pool<Shader>::iterator;

    Handle<Shader> add(Shader&&);
    Shader& get(Handle<Shader> const&);
    void remove(Handle<Shader> const&);

	void reload_shaders();

    iterator begin();
    iterator end();

private:
    Swapping_Pool<Shader> shaders;
};

#endif // !SHADERS_SHADER_MANAGER_HPP_INCLUDE