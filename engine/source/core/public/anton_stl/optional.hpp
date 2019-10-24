#ifndef CORE_ANTON_STL_OPTIONAL_HPP_INCLUDE
#define CORE_ANTON_STL_OPTIONAL_HPP_INCLUDE

#include <anton_stl/aligned_buffer.hpp>
#include <anton_stl/config.hpp>
#include <anton_stl/type_traits.hpp>
#include <intrinsics.hpp>

#include <new> // std::launder

namespace anton_engine::anton_stl {
    struct Null_Optional_Tag {
        explicit Null_Optional_Tag() = default;
    };

    constexpr Null_Optional_Tag null_optional;

    class Null_Optional_Access {};

    // TODO: Move ctor and assign should only be noexcept when T's are
    // TODO: hashing
    // TODO: emplace
    template <typename T>
    class Optional {
    public:
        using value_type = T;

        Optional(Null_Optional_Tag) = default;
        template <typename... Args>
        Optional(Variadic_Construct_Tag, Args&&...);
        Optional(Optional const&);
        // Leaves other in null_optional state
        Optional(Optional&& other) noexcept;
        ~Optional();

        void operator=(Optional const&);
        // Leaves other in null_optional state
        void operator=(Optional&& other) noexcept;

        [[nodiscard]] operator bool() const {
            return _holds_value;
        }

        [[nodiscard]] bool holds_value() const {
            return _holds_value;
        }

        [[nodiscard]] T* operator->();
        [[nodiscard]] T const* operator->() const;
        [[nodiscard]] T& operator*() &;
        [[nodiscard]] T const& operator*() const&;
        [[nodiscard]] T&& operator*() &&;
        [[nodiscard]] T const&& operator*() const&&;
        [[nodiscard]] T& value() &;
        [[nodiscard]] T const& value() const&;
        [[nodiscard]] T&& value() &&;
        [[nodiscard]] T const&& value() const&&;

        friend void swap(Optional&, Optional&) noexcept;

    private:
        Aligned_Buffer<sizeof(T), alignof(T)> _data;
        bool _holds_value = false;

        [[nodiscard]] T* get_pointer();
        [[nodiscard]] T const* get_pointer() const;
    };

    template <typename T>
    template <typename... Args>
    inline Optional<T>::Optional(Variadic_Construct_Tag, Args&&... args): _holds_value(true) {
        new (&data) T(anton_stl::forward<Args>(args)...);
    }

    template <typename T>
    inline Optional<T>::Optional(Optional const& other): _holds_value(other._holds_value) {
        if (other._holds_value) {
            new (&data) T(other.value());
        }
    }

    template <typename T>
    inline Optional<T>::Optional(Optional&& other) noexcept: _holds_value(other._holds_value) {
        if (other._holds_value) {
            new (&data) T(anton_stl::move(other).value());
            other.get_pointer()->~T();
            other._holds_value = false;
        }
    }

    template <typename T>
    inline Optional<T>::~Optional() {
        if (_holds_value) {
            get_pointer()->~T();
        }
    }

    template <typename T>
    inline void Optional<T>::operator=(Optional const& other) {
        if (_holds_value) {
            get_pointer()->~T();
        }

        _holds_value = other._holds_value;
        if (other._holds_value) {
            new (&data) T(other.value());
        }
    }

    template <typename T>
    inline void Optional<T>::operator=(Optional&& other) noexcept {
        if (_holds_value) {
            get_pointer()->~T();
        }

        _holds_value = other._holds_value;
        if (other._holds_value) {
            new (&data) T(anton_stl::move(other).value());
            other.get_pointer()->~T();
            other._holds_value = false;
        }
    }

    template <typename T>
    inline T* Optional<T>::operator->() {
        if (ANTON_LIKELY(_holds_value)) {
            return get_pointer();
        } else {
            throw Null_Optional_Access();
        }
    }

    template <typename T>
    inline T const* Optional<T>::operator->() const {
        if (ANTON_LIKELY(_holds_value)) {
            return get_pointer();
        } else {
            throw Null_Optional_Access();
        }
    }

    template <typename T>
    inline T& Optional<T>::operator*() & {
        return value();
    }

    template <typename T>
    inline T const& Optional<T>::operator*() const& {
        return value();
    }

    template <typename T>
    inline T&& Optional<T>::operator*() && {
        return value();
    }

    template <typename T>
    inline T const&& Optional<T>::operator*() const&& {
        return value();
    }

    template <typename T>
    inline T& Optional<T>::value() & {
        if (ANTON_LIKELY(_holds_value)) {
            return *get_pointer();
        } else {
            throw Null_Optional_Access();
        }
    }

    template <typename T>
    inline T const& Optional<T>::value() const& {
        if (ANTON_LIKELY(_holds_value)) {
            return *get_pointer();
        } else {
            throw Null_Optional_Access();
        }
    }

    template <typename T>
    inline T&& Optional<T>::value() && {
        if (ANTON_LIKELY(_holds_value)) {
            return anton_stl::move(*get_pointer());
        } else {
            throw Null_Optional_Access();
        }
    }

    template <typename T>
    inline T const&& Optional<T>::value() const&& {
        if (ANTON_LIKELY(_holds_value)) {
            return anton_stl::move(*get_pointer());
        } else {
            throw Null_Optional_Access();
        }
    }

    template <typename T>
    inline void swap(Optional<T>& lhs, Optional<T>& rhs) {
        if (lhs) {
            if (rhs) {
                swap(lhs.value(), rhs.value());
            } else {
                rhs = anton_stl::move(lhs);
            }
        } else if (rhs) {
            lhs = anton_stl::move(rhs);
        }
    }

    template <typename T>
    inline T* Optional<T>::get_pointer() {
        return std::launder(reinterpret_cast<T*>(&_data));
    }

    template <typename T>
    inline T const* Optional<T>::get_pointer() const {
        return std::launder(reinterpret_cast<T const*>(&_data));
    }
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_OPTIONAL_HPP_INCLUDE
