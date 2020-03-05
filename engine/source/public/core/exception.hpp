#ifndef CORE_EXCEPTION_HPP_INCLUDE
#define CORE_EXCEPTION_HPP_INCLUDE

#include <core/stl/string.hpp>
#include <core/stl/string_view.hpp>

namespace anton_engine {
    class Exception {
    public:
        Exception(anton_stl::String_View message): _message(message) {}

    private:
        anton_stl::String _message;
    };

    class Invalid_Argument_Exception: public Exception {
    public:
        using Exception::Exception;
    };
} // namespace anton_engine

#endif // !CORE_EXCEPTION_HPP_INCLUDE
