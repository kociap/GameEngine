#include <utils/filesystem.hpp>

#include <stdexcept>

namespace utils {
    std::filesystem::path concat_paths(std::filesystem::path const& a, std::filesystem::path const& b) {
        std::filesystem::path copy(a);
        copy /= b; // TODO use generic separator
        return copy;
    }

    int64_t get_file_size(std::ifstream& file) {
        auto pos = file.tellg();
        file.seekg(0, std::ios::end);
        int64_t size = file.tellg();
        file.seekg(pos);
        return size;
    }

    anton_stl::Vector<uint8_t> read_file_binary(std::filesystem::path const& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to read the file"); // TODO More specific exception that will actually allow
                                                                 //        me to dispaly useful information to the user
        }
        int64_t file_size = utils::get_file_size(file);
        anton_stl::Vector<uint8_t> file_contents(file_size);
        file.read(reinterpret_cast<char*>(file_contents.data()), file_size); // TODO reinterpret_cast
        return file_contents;
    }
} // namespace utils
