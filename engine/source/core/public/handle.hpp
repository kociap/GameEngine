#ifndef CORE_HANDLE_HPP_INCLUDE
#define CORE_HANDLE_HPP_INCLUDE

#include <cstdint>

template <typename T>
class Handle {
public:
    uint64_t value;

    explicit Handle(uint64_t val) : value(val) {}
};

#endif //! CORE_HANDLE_HPP_INCLUDE