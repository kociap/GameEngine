#ifndef CORE_HANDLE_HPP_INCLUDE
#define CORE_HANDLE_HPP_INCLUDE

template <typename T>
class Handle {
public:
    T* operator->() {}
};

#endif //! CORE_HANDLE_HPP_INCLUDE