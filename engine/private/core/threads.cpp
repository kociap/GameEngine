#include <core/threads.hpp>

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
  #error threads not implemented for non-windows builds.
#endif
