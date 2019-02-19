#ifndef CORE_HANDLE_HPP_INCLUDE
#define CORE_HANDLE_HPP_INCLUDE

#include <cstdint>

template <typename T>
class Handle {
public:
    uint64_t value;

    explicit Handle(uint64_t val) : value(val) {}
};

template <typename T>
bool operator==(Handle<T> const& a, Handle<T> const& b) {
    return a.value == b.value;
}

#endif //! CORE_HANDLE_HPP_INCLUDE