#ifndef CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE
#define CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <anton_stl/iterators.hpp>

namespace anton_stl {
    template <typename T>
    class Basic_String_View {
    public:
        using size_type = anton_stl::ssize_t;
        using difference_type = anton_stl::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;
        using reverse_iterator = anton_stl::reverse_iterator<iterator>;
        using const_reverse_iterator = anton_stl::reverse_iterator<const_iterator>;

    public:
        /* [[nodiscard]] */ constexpr Basic_String_View();
        /* [[nodiscard]] */ constexpr Basic_String_View(Basic_String_View const&) = default;
        /* [[nodiscard]] */ constexpr Basic_String_View(T const*, size_type);
        /* [[nodiscard]] */ constexpr Basic_String_View(T const*);
        Basic_String_View& operator=(Basic_String_View const&) = default;

        [[nodiscard]] constexpr const_reference operator[](size_type index) const;

        [[nodiscard]] constexpr const_iterator begin() const;
        [[nodiscard]] constexpr const_iterator cbegin() const;
        [[nodiscard]] constexpr const_iterator end() const;
        [[nodiscard]] constexpr const_iterator cend() const;
        [[nodiscard]] constexpr const_reverse_iterator rbegin() const;
        [[nodiscard]] constexpr const_reverse_iterator crbegin() const;
        [[nodiscard]] constexpr const_reverse_iterator rend() const;
        [[nodiscard]] constexpr const_reverse_iterator crend() const;

        [[nodiscard]] constexpr size_type size() const;
        [[nodiscard]] constexpr size_type max_size() const;

        [[nodiscard]] constexpr const_pointer data() const;

    private:
        T const* storage;
        size_type _size;
    };

    using String_View = Basic_String_View<char>;
} // namespace anton_stl

#include <anton_stl/string_view.tpp>

#endif // !CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE
