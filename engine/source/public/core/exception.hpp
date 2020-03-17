#ifndef CORE_EXCEPTION_HPP_INCLUDE
#define CORE_EXCEPTION_HPP_INCLUDE

#include <core/atl/string.hpp>
#include <core/atl/string_view.hpp>

namespace anton_engine {
    class Exception {
    public:
        Exception(atl::String_View message): _message(message) {}

        atl::String_View get_message() const {
            return _message;
        }

    private:
        atl::String _message;
    };

    class Invalid_Argument_Exception: public Exception {
    public:
        using Exception::Exception;
    };
} // namespace anton_engine

#endif // !CORE_EXCEPTION_HPP_INCLUDE
