#include <paths.hpp>
#include <paths_internal.hpp>

namespace paths {
    static std::filesystem::path _engine_executable_directory;
    static std::filesystem::path _project_directory;

    void set_engine_executable_directory(std::filesystem::path path) {
        _engine_executable_directory = std::move(path);
    }

    void set_project_directory(std::filesystem::path path) {
        _project_directory = std::move(path);
    }

    std::filesystem::path engine_executable_directory() {
        return _engine_executable_directory;
    }

    std::filesystem::path project_directory() {
        return _project_directory;
    }
} // namespace paths