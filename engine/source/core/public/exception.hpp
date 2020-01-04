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

    class Invalid_Argument_Exception: public Exception {
    public:
        using Exception::Exception;
    };
} // namespace anton_engine

#endif // !CORE_EXCEPTION_HPP_INCLUDE
