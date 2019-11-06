#ifndef CORE_ANTON_STL_OPTIONAL_HPP_INCLUDE
#define CORE_ANTON_STL_OPTIONAL_HPP_INCLUDE

#include <anton_assert.hpp>
#include <anton_stl/aligned_buffer.hpp>
#include <anton_stl/config.hpp>
#include <anton_stl/memory.hpp>
#include <anton_stl/type_traits.hpp>
#include <intrinsics.hpp>

#include <new> // std::launder

namespace anton_engine::anton_stl {
    struct Null_Optional_Tag {
        explicit Null_Optional_Tag() = default;
    };

    constexpr Null_Optional_Tag null_optional;

    class Null_Optional_Access {};

    namespace detail {
        template <typename T, bool = anton_stl::is_trivially_destructible<T>>
        class Optional_Destruct_Base {
        public:
            Optional_Destruct_Base(): _null_state(), _holds_value(false) {}
            template <typename... Args>
            Optional_Destruct_Base(Variadic_Construct_Tag, Args&&... args): _value(anton_stl::forward<Args>(args)...), _holds_value(true) {}
            ~Optional_Destruct_Base() = default;

            void destruct() {
                if (_holds_value) {
                    _holds_value = false;
                }
            }

            union {
                char _null_state;
                T _value;
            };
            bool _holds_value;
        };

        template <typename T>
        class Optional_Destruct_Base<T, false> {
        public:
            Optional_Destruct_Base(): _null_state(), _holds_value(false) {}

            template <typename... Args>
            Optional_Destruct_Base(Variadic_Construct_Tag, Args&&... args): _value(anton_stl::forward<Args>(args)...), _holds_value(true) {}

            ~Optional_Destruct_Base() {
                if (holds_value) {
                    _value.~T();
                }
            }

            void destruct() {
                if (_holds_value) {
                    _value.~T();
                    _holds_value = false;
                }
            }

            union {
                char _null_state;
                T _value;
            };
            bool _holds_value;
        };

        template <typename T>
        class Optional_Storage_Base: public Optional_Destruct_Base<T> {
        public:
            using Optional_Destruct_Base<T>::Optional_Destruct_Base;

            bool holds_value() const {
                return this->_holds_value;
            }

            T& get() & {
                return this->_value;
            }

            T const& get() const& {
                return this->_value;
            }

            T&& get() && {
                return anton_stl::move(this->_value);
            }

            T const&& get() const&& {
                return anton_stl::move(this->_value);
            }

            template <typename... Args>
            void construct(Args&&... args) {
                ANTON_ASSERT(!has_value(), "construct called on empty Optional.");
                ::new (anton_stl::addressof(this->_value)) T(anton_stl::forward<Args>(args)...);
                this->_holds_value = true;
            }

            void assign(Optional_Storage_Base const& opt) {
                if (holds_value()) {
                    if (opt.holds_value()) {
                        this->_value = opt.get();
                    } else {
                        destruct();
                    }
                } else {
                    if (opt.holds_value()) {
                        construct(opt.get());
                    }
                }
            }

            void assign(Optional_Storage_Base&& opt) {
                if (holds_value()) {
                    if (opt.holds_value()) {
                        this->_value = anton_stl::move(opt).get();
                        opt.destruct();
                    } else {
                        destruct();
                    }
                } else {
                    if (opt.holds_value()) {
                        construct(anton_stl::move(opt).get());
                        opt.destruct();
                    }
                }
            }
        };

        template <typename T, bool = anton_stl::is_trivially_copy_constructible<T>>
        class Optional_Copy_Base: public Optional_Storage_Base<T> {
        public:
            using Optional_Storage_Base<T>::Optional_Storage_Base;
        };

        template <typename T>
        class Optional_Copy_Base<T, false>: public Optional_Storage_Base<T> {
        public:
            using Optional_Storage_Base<T>::Optional_Storage_Base;

            Optional_Copy_Base() = default;

            Optional_Copy_Base(Optional_Copy_Base const& other) {
                if (other.holds_value()) {
                    construct(other.get());
                }
            }

            Optional_Copy_Base(Optional_Copy_Base&&) noexcept = default;
            Optional_Copy_Base& operator=(Optional_Copy_Base const&) = default;
            Optional_Copy_Base& operator=(Optional_Copy_Base&&) noexcept = default;
        };

        template <typename T, bool = anton_stl::is_trivially_move_constructible<T>>
        class Optional_Move_Base: public Optional_Copy_Base<T> {
        public:
            using Optional_Copy_Base<T>::Optional_Copy_Base;
        };

        template <typename T>
        class Optional_Move_Base<T, false>: public Optional_Copy_Base<T> {
        public:
            using Optional_Copy_Base<T>::Optional_Copy_Base;

            Optional_Move_Base() = default;
            Optional_Move_Base(Optional_Move_Base const&) = default;

            Optional_Move_Base(Optional_Move_Base&& other) noexcept {
                if (other.holds_value()) {
                    construct(anton_stl::move(other).get());
                    other.destruct();
                }
            }

            Optional_Move_Base& operator=(Optional_Move_Base const&) = default;
            Optional_Move_Base& operator=(Optional_Move_Base&&) noexcept = default;
        };

        template <typename T, bool = anton_stl::is_trivially_copy_assignable<T>>
        class Optional_Copy_Assign_Base: public Optional_Move_Base<T> {
        public:
            using Optional_Move_Base<T>::Optional_Move_Base;
        };

        template <typename T>
        class Optional_Copy_Assign_Base<T, false>: public Optional_Move_Base<T> {
        public:
            using Optional_Move_Base<T>::Optional_Move_Base;

            Optional_Copy_Assign_Base() = default;
            Optional_Copy_Assign_Base(Optional_Copy_Assign_Base const&) = default;
            Optional_Copy_Assign_Base(Optional_Copy_Assign_Base&&) noexcept = default;

            Optional_Copy_Assign_Base& operator=(Optional_Copy_Assign_Base const& other) {
                assign(other);
                return *this;
            }

            Optional_Copy_Assign_Base& operator=(Optional_Copy_Assign_Base&&) noexcept = default;
        };

        template <typename T, bool = anton_stl::is_trivially_move_assignable<T>>
        class Optional_Move_Assign_Base: public Optional_Copy_Assign_Base<T> {
        public:
            using Optional_Copy_Assign_Base<T>::Optional_Copy_Assign_Base;
        };

        template <typename T>
        class Optional_Move_Assign_Base<T, false>: public Optional_Copy_Assign_Base<T> {
        public:
            using Optional_Copy_Assign_Base<T>::Optional_Copy_Assign_Base;

            Optional_Move_Assign_Base() = default;
            Optional_Move_Assign_Base(Optional_Move_Assign_Base const&) = default;
            Optional_Move_Assign_Base(Optional_Move_Assign_Base&&) noexcept = default;
            Optional_Move_Assign_Base& operator=(Optional_Move_Assign_Base const&) = default;

            Optional_Move_Assign_Base& operator=(Optional_Move_Assign_Base&& other) noexcept {
                assign(anton_stl::move(other));
                other.destruct();
                return *this;
            }
        };

    } // namespace detail

    // TODO: hashing
    // TODO: emplace
    template <typename T>
    class Optional: private detail::Optional_Move_Assign_Base<T> {
    private:
        using _base = detail::Optional_Move_Assign_Base<T>;

        template <typename U>
        struct Is_Constructible_From_Type {
            static constexpr bool value =
                conjunction<Negation<Is_Same<Optional<T>, remove_const_ref<U>>>, Negation<Is_Same<Variadic_Construct_Tag, remove_const_ref<U>>>,
                            Negation<Is_Same<Null_Optional_Tag, remove_const_ref<U>>>, Is_Constructible<T, U>>;
        };

    public:
        using value_type = T;

        Optional(Null_Optional_Tag) {}

        template <typename... Args, typename = enable_if<is_constructible<T, Args...>>>
        explicit Optional(Variadic_Construct_Tag, Args&&... args): _base(variadic_construct, anton_stl::forward<Args>(args)...) {}

        template <typename U>
        Optional(U&& arg, enable_if<Is_Constructible_From_Type<U&&>::value && is_convertible<U&&, T>, int> = 0)
            : _base(variadic_construct, anton_stl::forward<U>(arg)) {}

        template <typename U>
        explicit Optional(U&& arg, enable_if<Is_Constructible_From_Type<U&&>::value && !is_convertible<U&&, T>, int> = 0)
            : _base(variadic_construct, anton_stl::forward<U>(arg)) {}

        Optional(Optional const&) = default;
        // Leaves other in null_optional state
        Optional(Optional&& other) noexcept = default;
        Optional& operator=(Optional const&) = default;
        // Leaves other in null_optional state
        Optional& operator=(Optional&& other) noexcept = default;
        ~Optional() = default;

        [[nodiscard]] operator bool() const {
            return _base::holds_value();
        }

        [[nodiscard]] bool holds_value() const {
            return _base::holds_value();
        }

        [[nodiscard]] T* operator->() {
            ANTON_ASSERT(holds_value(), "operator-> called on empty Optional.");
            return anton_stl::addressof(this->get());
        }

        [[nodiscard]] T const* operator->() const {
            ANTON_ASSERT(holds_value(), "operator-> called on empty Optional.");
            return anton_stl::addressof(this->get());
        }

        [[nodiscard]] T& operator*() & {
            ANTON_ASSERT(holds_value(), "operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T const& operator*() const& {
            ANTON_ASSERT(holds_value(), "operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T&& operator*() && {
            ANTON_ASSERT(holds_value(), "operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T const&& operator*() const&& {
            ANTON_ASSERT(holds_value(), "operator* called on empty Optional.");
            return this->get();
        }

        [[nodiscard]] T& value() & {
            if (ANTON_LIKELY(holds_value())) {
                return this->get();
            } else {
                throw Null_Optional_Access();
            }
        }

        [[nodiscard]] T const& value() const& {
            if (ANTON_LIKELY(holds_value())) {
                return this->get();
            } else {
                throw Null_Optional_Access();
            }
        }

        [[nodiscard]] T&& value() && {
            if (ANTON_LIKELY(holds_value())) {
                return this->get();
            } else {
                throw Null_Optional_Access();
            }
        }

        [[nodiscard]] T const&& value() const&& {
            if (ANTON_LIKELY(holds_value())) {
                return this->get();
            } else {
                throw Null_Optional_Access();
            }
        }
    };

    template <typename T>
    inline void swap(Optional<T>& lhs, Optional<T>& rhs) {
        if (lhs.holds_value()) {
            if (rhs.holds_value()) {
                swap(*lhs, *rhs);
            } else {
                rhs = anton_stl::move(lhs);
            }
        } else {
            if (rhs.holds_value()) {
                lhs = anton_stl::move(rhs);
            }
        }
    }
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_OPTIONAL_HPP_INCLUDE
