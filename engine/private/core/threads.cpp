#include <core/threads.hpp>

#include <anton/assert.hpp>

#include <thread>

#if defined(_WIN32) || defined(_WIN64)

extern "C" {
// xthreads.h

struct xtime;

void _Thrd_sleep(const xtime*);
}

namespace anton_engine::threads {
  void sleep(Timespec const& duration)
  {
    _Thrd_sleep(reinterpret_cast<xtime const*>(&duration));
  }
} // namespace anton_engine::threads

#else

  #include <time.h>

namespace anton_engine::threads {
  void sleep(Timespec const& duration)
  {
    ANTON_ASSERT(duration.nanoseconds >= 0 && duration.seconds >= 0,
                 "duration must be a positive number");
    ANTON_ASSERT(duration.nanoseconds < 1000000000,
                 "nanoseconds must be less than 1 billion (1 second)");
    timespec requested{duration.seconds, duration.nanoseconds};
    timespec remaining{};
    int result = nanosleep(&requested, &remaining);
    ANTON_UNUSED(result);
  }
} // namespace anton_engine::threads

#endif
