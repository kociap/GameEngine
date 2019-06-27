#include <file.hpp>

#include <fstream>

File::File() {}

File::File(std::string_view filename) {
    read_file(filename);
}

File::File(std::filesystem::path filename) {
    read_file(filename);
}

void File::read_file(std::string_view filename) {
    std::ifstream f(filename.data());
    f.seekg(std::ios::end);
    uint64_t file_size = f.tellg();
    f.seekg(std::ios::beg);
    contents.resize(file_size);
    f.read(contents.data(), file_size);
}

void File::read_file(std::filesystem::path filename) {
    std::ifstream f(filename);
    f.seekg(0, std::ios::end);
    uint64_t file_size = f.tellg();
    f.seekg(0, std::ios::beg);
    contents.resize(file_size);
    f.read(contents.data(), file_size);
}

void File::write_file(std::string_view filename) {
    std::ofstream f(filename.data());
    f.write(contents.data(), contents.size());
}

File::iterator File::begin() {
    return contents.begin();
}

File::iterator File::end() {
    return contents.end();
}
