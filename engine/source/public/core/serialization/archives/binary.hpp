#pragma once

#include <anton/stream.hpp>
#include <core/types.hpp>

namespace anton_engine::serialization {
  class Binary_Input_Archive {
  public:
    explicit Binary_Input_Archive(anton::Input_Stream& strm): file(strm) {}

    template<typename T>
    void read(T& v)
    {
      constexpr usize data_size = sizeof(T);
      void* out = reinterpret_cast<void*>(&v);
      read_binary(out, data_size);
    }

    void read_binary(void*, i64 bytes);

  private:
    anton::Input_Stream& file;
  };

  class Binary_Output_Archive {
  public:
    explicit Binary_Output_Archive(anton::Output_Stream& strm): file(strm) {}

    template<typename T>
    void write(T const& v)
    {
      constexpr usize data_size = sizeof(T);
      void const* data = reinterpret_cast<void const*>(&v);
      write_binary(data, data_size);
    }

    void write_binary(void const*, i64 bytes);

  private:
    anton::Output_Stream& file;
  };
} // namespace anton_engine::serialization
