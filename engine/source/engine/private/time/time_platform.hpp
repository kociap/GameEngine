#ifndef ENGINE_TIME_TIME_PLATFORM_HPP_INCLUDE
#define ENGINE_TIME_TIME_PLATFORM_HPP_INCLUDE

#include <cstdint>

uint64_t get_performance_counter();
uint64_t get_performance_frequency();
// Returns time in seconds
double get_time();

#endif // !ENGINE_TIME_TIME_PLATFORM_HPP_INCLUDE
