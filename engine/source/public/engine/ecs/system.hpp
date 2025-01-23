#pragma once

namespace anton_engine {
  class System {
  public:
    virtual ~System() {}

    virtual void start() {}
    virtual void update() = 0;
  };
} // namespace anton_engine
