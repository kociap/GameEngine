#pragma once

#include <anton/array.hpp>
#include <filesystem>
#include <string_view>

namespace anton_engine {
  class File {
  public:
    using iterator = anton::Array<char>::iterator;

    File();
    File(std::string_view filename);
    File(std::filesystem::path filename);

    void read_file(std::string_view filename);
    void read_file(std::filesystem::path filename);
    void write_file(std::string_view filename);

    iterator begin();
    iterator end();

  private:
    anton::Array<char> contents;
  };
} // namespace anton_engine
