#ifndef CORE_ANTON_STL_DETAIL_TRAITS_TRANSFORMATIONS_HPP_INCLUDE
#define CORE_ANTON_STL_DETAIL_TRAITS_TRANSFORMATIONS_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <anton_stl/detail/traits_base.hpp>
#include <anton_stl/detail/traits_common.hpp>
#include <anton_stl/detail/traits_function.hpp>

namespace anton_engine::anton_stl {
    // Type_Identity
    //
    template <typename T>
    struct Type_Identity {
        using type = T;
    };

    template <typename T>
    using type_identity = typename Type_Identity<T>::type;

    // Remove_Extent
    //
    template <typename T>
    struct Remove_Extent {
        using type = T;
    };

    template <typename T>
    struct Remove_Extent<T[]> {
        using type = T;
    };

    template <typename T, anton_stl::size_t N>
    struct Remove_Extent<T[N]> {
        using type = T;
    };

    template <typename T>
    using remove_extent = typename Remove_Extent<T>::type;

    // Remove_All_Extents
    //
    template <typename T>
    struct Remove_All_Extents {
        using type = T;
    };

    template <typename T>
    struct Remove_All_Extents<T[]> {
        using type = typename Remove_All_Extents<T>::type;
    };

    template <typename T, anton_stl::size_t N>
    struct Remove_All_Extents<T[N]> {
        using type = typename Remove_All_Extents<T>::type;
    };

    template <typename T>
    using remove_all_extents = typename Remove_All_Extents<T>::type;

    template <typename T>
    struct Remove_Reference {
        using type = T;
    };

    template <typename T>
    struct Remove_Reference<T&> {
        using type = T;
    };

    template <typename T>
    struct Remove_Reference<T&&> {
        using type = T;
    };

    template <typename T>
    using remove_reference = typename Remove_Reference<T>::type;

    template <typename T>
    struct Remove_Pointer {
        using type = T;
    };

    template <typename T>
    struct Remove_Pointer<T*> {
        using type = T;
    };

    template <typename T>
    struct Remove_Pointer<T* const> {
        using type = T;
    };

    template <typename T>
    using remove_pointer = typename Remove_Pointer<T>::type;

    template <typename T>
    struct Remove_Const {
        using type = T;
    };

    template <typename T>
    struct Remove_Const<T const> {
        using type = T;
    };

    template <typename T>
    using remove_const = typename Remove_Const<T>::type;

    template <typename T>
    struct Remove_Const_Ref {
        using type = remove_const<remove_reference<T>>;
    };

    template <typename T>
    using remove_const_ref = typename Remove_Const_Ref<T>::type;

    namespace detail {
        template <typename T, typename = void>
        struct Add_Reference {
            using lvalue = T;
            using rvalue = T;
        };

        template <typename T>
        struct Add_Reference<T, void_trait<T&>> {
            using lvalue = T&;
            using rvalue = T&&;
        };
    } // namespace detail

    // Add_Lvalue_Reference
    //
    template <typename T>
    struct Add_Lvalue_Reference {
        using type = typename detail::Add_Reference<T>::lvalue;
    };

    template <typename T>
    using add_lvalue_reference = typename Add_Lvalue_Reference<T>::type;

    // Add_Rvalue_Reference
    //
    template <typename T>
    struct Add_Rvalue_Reference {
        using type = typename detail::Add_Reference<T>::rvalue;
    };

    template <typename T>
    using add_rvalue_reference = typename Add_Rvalue_Reference<T>::type;

    // Add_Pointer
    //
    template <typename T>
    struct Add_Pointer {
    private:
        template <typename U>
        static auto test(int) -> anton_stl::Type_Identity<anton_stl::remove_reference<T>*>;

        template <typename U>
        static auto test(...) -> anton_stl::Type_Identity<T>;

    public:
        using type = typename decltype(test<T>(0))::type;
    };

    template <typename T>
    using add_pointer = typename Add_Pointer<T>::type;

    template <typename T>
    struct Add_Const {
        using type = T const;
    };

    template <typename T>
    using add_const = typename Add_Const<T>::type;

    // Decay
    //
    template <typename T>
    struct Decay {
    private:
        using U = anton_stl::remove_reference<T>;

    public:
        using type = anton_stl::conditional<anton_stl::detail::Is_Array<U>::value, anton_stl::remove_extent<U>*,
                                            anton_stl::conditional<anton_stl::is_function<T>, anton_stl::add_pointer<T>, anton_stl::remove_const<T>>>;
    };

    template <typename T>
    using decay = typename Decay<T>::type;
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_DETAIL_TRAITS_TRANSFORMATIONS_HPP_INCLUDE
