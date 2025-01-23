#pragma once

#include <core/exception.hpp>

namespace anton_engine {
  class Shader_Not_Created: public Exception {
  public:
    using Exception::Exception;
  };

  class Shader_Compilation_Failed: public Exception {
  public:
    using Exception::Exception;
  };

  class Program_Not_Created: public Exception {
  public:
    using Exception::Exception;
  };

  class Program_Linking_Failed: public Exception {
  public:
    using Exception::Exception;
  };
} // namespace anton_engine
