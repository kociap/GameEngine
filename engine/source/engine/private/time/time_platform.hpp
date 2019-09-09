#ifndef ENGINE_TIME_TIME_PLATFORM_HPP_INCLUDE
#define ENGINE_TIME_TIME_PLATFORM_HPP_INCLUDE

#include <cstdint>

struct System_Time {
    uint16_t year;
    // 1 through 12
    uint16_t month;
    // 1 through 31
    uint16_t day;
    // 0 (Sunday) through 6 (Saturday)
    uint16_t day_of_week;
    // 0 through 23
    uint16_t hour;
    // 0 through 59
    uint16_t minutes;
    // 0 through 59
    uint16_t seconds;
    // 0 through 999
    uint16_t milliseconds;
};

uint64_t get_performance_counter();
uint64_t get_performance_frequency();
// Returns time in seconds
double get_time();
System_Time get_utc_system_time();
System_Time get_local_system_time();

#endif // !ENGINE_TIME_TIME_PLATFORM_HPP_INCLUDE
