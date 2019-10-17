#ifndef ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
#define ENGINE_RESOURCE_MANAGER_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <handle.hpp>

namespace anton_engine {
    template <typename T>
    class Resource_Manager {
    public:
        using iterator = typename anton_stl::Vector<T>::iterator;

        iterator begin();
        iterator end();

        Handle<T> add(T&&);
        T& get(Handle<T>);
        T const& get(Handle<T>) const;
        void remove(Handle<T>);

    private:
        anton_stl::Vector<T> resources;
        anton_stl::Vector<uint64_t> identifiers;
    };
} // namespace anton_engine

#include <resource_manager.tpp>

#endif // !ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
