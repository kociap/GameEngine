#ifndef CORE_SINGLETON_HPP_INCLUDE
#define CORE_SINGLETON_HPP_INCLUDE

#include "debug_macros.hpp"

template <typename T>
class Singleton {
private:
    static T* instance;

public:
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    Singleton() {
        GE_assert(!instance, "Attempting to create multiple singleton instances");
        instance = static_cast<T*>(this);
    }

    ~Singleton() {
        instance = nullptr;
    }

    static T& get() {
        return *instance;
    }

    static T* get_ptr() {
        GE_assert(instance, "Singleton instance does not exist");
        return instance;
    }
};

template<typename T>
T* Singleton<T>::instance = nullptr;

#endif // !CORE_SINGLETON_HPP_INCLUDE