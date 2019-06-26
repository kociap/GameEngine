#ifndef ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
#define ENGINE_RESOURCE_MANAGER_HPP_INCLUDE

#include <containers/vector.hpp>
#include <handle.hpp>
#include <config.hpp>

template <typename T>
class Resource_Manager {
public:
    using iterator = typename containers::Vector<T>::iterator;

    iterator begin();
    iterator end();

    Handle<T> add(T&&);
    T& get(Handle<T>);
    void remove(Handle<T>);

private:
    containers::Vector<T> resources;
    containers::Vector<id_type> identifiers;
};

#include "resource_manager.tpp"

#endif // !ENGINE_RESOURCE_MANAGER_HPP_INCLUDE