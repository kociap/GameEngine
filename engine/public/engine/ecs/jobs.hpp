#pragma once

#include <core/types.hpp>

namespace anton_engine {
  class Job {
  public:
    virtual ~Job() {}

    virtual void execute() = 0;
  };

  i64 schedule_job(Job*);
} // namespace anton_engine
