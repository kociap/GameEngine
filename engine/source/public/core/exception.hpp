#pragma once

#include <anton/string.hpp>
#include <anton/string_view.hpp>

namespace anton_engine {
  class Exception {
  public:
    Exception(anton::String_View message): _message(message) {}

    anton::String_View get_message() const
    {
      return _message;
    }

  private:
    anton::String _message;
  };

  class Invalid_Argument_Exception: public Exception {
  public:
    using Exception::Exception;
  };
} // namespace anton_engine
