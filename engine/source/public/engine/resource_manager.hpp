#ifndef ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
#define ENGINE_RESOURCE_MANAGER_HPP_INCLUDE

#include <core/atl/vector.hpp>
#include <core/handle.hpp>

namespace anton_engine {
    template <typename T>
    class Resource_Manager {
    public:
        using iterator = typename atl::Vector<T>::iterator;

        iterator begin();
        iterator end();

        Handle<T> add(T&&);
        T& get(Handle<T>);
        T const& get(Handle<T>) const;
        void remove(Handle<T>);

    private:
        atl::Vector<T> resources;
        atl::Vector<uint64_t> identifiers;
    };
} // namespace anton_engine

#include <engine/resource_manager.tpp>

#endif // !ENGINE_RESOURCE_MANAGER_HPP_INCLUDE
