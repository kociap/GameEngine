#include "utils/path.hpp"

namespace utils {
    std::filesystem::path concat_paths(std::filesystem::path const& a, std::filesystem::path const& b) {
        std::filesystem::path copy(a);
        copy /= b;
        return copy;
    }
}