#include <core/serialization/archives/binary.hpp>

namespace anton_engine::serialization {
    void Binary_Input_Archive::read_binary(void* p, isize bytes) {
        char* ptr = reinterpret_cast<char*>(p);
        file.read(ptr, bytes);
    }

    void Binary_Output_Archive::write_binary(void const* p, isize bytes) {
        char const* ptr = reinterpret_cast<char const*>(p);
        file.write(ptr, bytes);
    }
} // namespace anton_engine::serialization
