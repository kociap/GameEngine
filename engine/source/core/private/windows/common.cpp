#include <windows/common.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace anton_engine::windows {
    uint64_t get_last_error() {
        return GetLastError();
    }

    anton_stl::String get_last_error_message() {
        DWORD error_code = GetLastError();
        if (error_code == 0) {
            return {};
        }

        LPWSTR message_buffer = nullptr;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code,
                       MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<LPWSTR>(&message_buffer), 0, nullptr);
        anton_stl::String error_message = anton_stl::String::from_utf16(reinterpret_cast<char16_t*>(message_buffer));
        LocalFree(message_buffer);
        return error_message;
    }
} // namespace anton_engine::windows