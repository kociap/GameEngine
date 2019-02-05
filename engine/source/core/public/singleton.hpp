#ifndef CORE_SINGLETON_HPP_INCLUDE
#define CORE_SINGLETON_HPP_INCLUDE

#include <cassert>

template <typename T>
class Singleton {
private:
    static T* instance = nullptr;

public:
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    Singleton() {
        assert(!instance, "Attempting to create multiple singleton instances");
        instance = static_cast<T*>(this);
    }

    ~Singleton() {
        instance = nullptr;
    }

    static T& get() {
        return *instance;
    }

    static T* get_ptr() {
        assert(instance, "Singleton instance does not exist");
        return instance;
    }
};

#endif // !CORE_SINGLETON_HPP_INCLUDE