#ifndef CORE_EXCEPTION_HPP_INCLUDE
#define CORE_EXCEPTION_HPP_INCLUDE

#include <anton_stl/string.hpp>
#include <anton_stl/string_view.hpp>

namespace anton_engine {
    class Exception {
    public:
        Exception(anton_stl::String_View message): _message(message) {}

    private:
        anton_stl::String_View _message;
    };
} // namespace anton_engine

#endif // !CORE_EXCEPTION_HPP_INCLUDE
