#ifndef CORE_SERIALIZATION_ACHIVES_BINARY_HPP_INCLUDE
#define CORE_SERIALIZATION_ACHIVES_BINARY_HPP_INCLUDE

#include <core/types.hpp>
#include <fstream>
#include <new>

namespace anton_engine::serialization {
    class Binary_Input_Archive {
    public:
        explicit Binary_Input_Archive(std::ifstream& strm): file(strm) {}

        template <typename T>
        void read(T& v) {
            constexpr usize data_size = sizeof(T);
            char* data = reinterpret_cast<char*>(&v);
            file.read(data, data_size);
        }

        void read_binary(void*, isize bytes);

    private:
        std::ifstream& file;
    };

    class Binary_Output_Archive {
    public:
        explicit Binary_Output_Archive(std::ofstream& strm): file(strm) {}

        template <typename T>
        void write(T const& v) {
            constexpr usize data_size = sizeof(T);
            char const* data = reinterpret_cast<char const*>(&v);
            file.write(data, data_size);
        }

        void write_binary(void const*, isize bytes);

    private:
        std::ofstream& file;
    };
} // namespace anton_engine::serialization

#endif // !CORE_SERIALIZATION_ACHIVES_BINARY_HPP_INCLUDE
