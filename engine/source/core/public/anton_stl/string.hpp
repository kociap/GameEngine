#ifndef CORE_ANTON_STL_STRING_HPP_INCLUDE
#define CORE_ANTON_STL_STRING_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <iterators.hpp>
#include <memory>

namespace anton_stl {
    template <typename T, typename Allocator = std::allocator<T>>
    class basic_string {
    public:
        using size_t = anton_size_t;
        using difference_type = anton_ptrdiff_t;
        using allocator_type = Allocator;
        using value_type = T;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;
        using reverse_iterator = anton_stl::reverse_iterator<iterator>;
        using const_reverse_iterator = anton_stl::reverse_iterator<const_iterator>;
    };

    using string = basic_string<char>;
} // namespace anton_stl

#endif // !CORE_ANTON_STL_STRING_HPP_INCLUDE
