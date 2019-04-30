#ifndef CORE_HANDLE_HPP_INCLUDE
#define CORE_HANDLE_HPP_INCLUDE

#include <cstdint>

template <typename T>
class Handle {
public:
    uint64_t value = static_cast<uint64_t>(-1);
};

template <typename T>
bool operator==(Handle<T> const& a, Handle<T> const& b) {
    return a.value == b.value;
}

template <typename T>
bool operator!=(Handle<T> const& a, Handle<T> const& b) {
    return a.value != b.value;
}

#endif //! CORE_HANDLE_HPP_INCLUDE