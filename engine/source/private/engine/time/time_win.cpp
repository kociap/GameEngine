#include <engine/time.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace anton_engine {
    static u64 get_performance_counter() {
        u64 counts;
        // The output value is always nonzero and we may safely ignore the return value since:
        //   "On systems that run Windows XP or later, the function will always succeed and will thus never return zero."
        //   https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counts));
        return counts;
    }

    static u64 get_win_performance_frequency() {
        u64 frequency;
        // The output value is always nonzeroand we may safely ignore the return value since:
        //   "On systems that run Windows XP or later, the function will always succeed and will thus never return zero."
        //   https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
        return frequency;
    }

    static u64 get_performance_frequency() {
        static u64 frequency = get_win_performance_frequency();
        return frequency;
    }

    double get_time() {
        return static_cast<double>(get_performance_counter()) / static_cast<double>(get_performance_frequency());
    }

    System_Time get_utc_system_time() {
        SYSTEMTIME t = {};
        GetSystemTime(&t);
        return {t.wYear, t.wMonth, t.wDay, t.wDayOfWeek, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds};
    }

    System_Time get_local_system_time() {
        SYSTEMTIME t = {};
        GetLocalTime(&t);
        return {t.wYear, t.wMonth, t.wDay, t.wDayOfWeek, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds};
    }
} // namespace anton_engine
