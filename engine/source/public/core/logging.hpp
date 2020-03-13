#ifndef CORE_LOGGING_LOGGING_HPP_INCLUDE
#define CORE_LOGGING_LOGGING_HPP_INCLUDE

#include <core/atl/string_view.hpp>

namespace anton_engine {
    enum class Log_Message_Severity {
        info,
        warning,
        error,
        fatal_error,
    };

    void log_message(Log_Message_Severity, atl::String_View message);

#define ANTON_LOG_INFO(message) ::anton_engine::log_message(::anton_engine::Log_Message_Severity::info, message);
#define ANTON_LOG_WARNING(message) ::anton_engine::log_message(::anton_engine::Log_Message_Severity::warning, message);
#define ANTON_LOG_ERROR(message) ::anton_engine::log_message(::anton_engine::Log_Message_Severity::error, message);
} // namespace anton_engine

#endif // !CORE_LOGGING_LOGGING_HPP_INCLUDE
