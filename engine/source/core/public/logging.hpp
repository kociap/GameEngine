#ifndef CORE_LOGGING_LOGGING_HPP_INCLUDE
#define CORE_LOGGING_LOGGING_HPP_INCLUDE

#include <anton_stl/string_view.hpp>

namespace anton_engine {
    enum class Log_Message_Severity {
        info,
        warning,
        error,
        fatal_error,
    };

    void log_message(Log_Message_Severity, anton_stl::String_View message);
} // namespace anton_engine

#endif // !CORE_LOGGING_LOGGING_HPP_INCLUDE
