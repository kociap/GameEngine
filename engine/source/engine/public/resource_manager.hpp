#ifndef ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
#define ENGINE_RESOURCE_MANAGER_HPP_INCLUDE

#include "containers/vector.hpp"
#include "handle.hpp"

template <typename T>
class Resource_Manager {
public:
    Handle<T> add(T&&);
    T& get(Handle<T>);
    void remove(Handle<T>);

private:
    containers::Vector<T> resources;
};

#include "resource_manager.tpp"

#endif // !ENGINE_RESOURCE_MANAGER_HPP_INCLUDE