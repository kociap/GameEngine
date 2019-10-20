#ifndef CORE_ANTON_STL_SLICE_HPP_INCLUDE
#define CORE_ANTON_STL_SLICE_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/config.hpp>
#include <anton_stl/iterators.hpp>
#include <anton_stl/type_traits.hpp>
#include <anton_stl/utility.hpp>

namespace anton_engine::anton_stl {
    template <typename T>
    class Slice;

    template <typename T>
    struct Is_Slice: False_Type {};

    template <typename T>
    struct Is_Slice<Slice<T>>: True_Type {};

    template <typename T>
    struct Is_Slice<Slice<T> const>: True_Type {};

    template <typename T>
    constexpr bool is_slice = Is_Slice<T>::value;

    template <typename T>
    class Slice {
    public:
        using value_type = T;
        using size_type = isize;
        using difference_type = anton_stl::ptrdiff_t;
        using iterator = T*;
        using const_iterator = T const*;

        Slice(): _data(nullptr), _size(0) {}
        Slice(T* const first, size_type const length): _data(first), _size(length) {}
        Slice(T* const first, T* const last): _data(first), _size(last - first) {}
        template <typename Container>
        Slice(Container& c, disable_if<is_slice<Container>, void*> = nullptr): _data(anton_stl::data(c)), _size(anton_stl::size(c)) {}
        Slice(Slice const& other): _data(other._data), _size(other._size) {}

        void operator=(Slice const& other) {
            _data = other._data;
            _size = other._size;
        }

        T& operator[](size_type i) const {
            return *(_data + i);
        }

        iterator begin() const {
            return _data;
        }

        const_iterator cbegin() const {
            return _data;
        }

        iterator end() const {
            return _data + _size;
        }

        const_iterator cend() const {
            return _data + _size;
        }

        size_type size() const {
            return _size;
        }

        T* data() const {
            return _data;
        }

        Slice subslice(size_type first, size_type last) {
            return {_data + first, _data + last};
        }

        friend void swap(Slice& lhs, Slice& rhs) {
            swap(lhs._data, rhs._data);
            swap(lhs._size, rhs._size);
        }

    private:
        T* _data;
        size_type _size;
    };
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_SLICE_HPP_INCLUDE
