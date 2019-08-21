#ifndef ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
#define ENGINE_RESOURCE_MANAGER_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <config.hpp>
#include <handle.hpp>

template <typename T>
class Resource_Manager {
public:
    using iterator = typename anton_stl::Vector<T>::iterator;

    iterator begin();
    iterator end();

    Handle<T> add(T&&);
    T& get(Handle<T>);
    void remove(Handle<T>);

private:
    anton_stl::Vector<T> resources;
    anton_stl::Vector<id_type> identifiers;
};

#include <resource_manager.tpp>

#endif // !ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
