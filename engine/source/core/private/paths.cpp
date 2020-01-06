#include <paths.hpp>
#include <paths_internal.hpp>
#include <utils/filesystem.hpp>

namespace anton_engine::paths {
    static std::filesystem::path _executable_name;
    static std::filesystem::path _executable_directory;

    void set_executable_name(std::filesystem::path name) {
        _executable_name = std::move(name);
    }

    void set_executable_directory(std::filesystem::path path) {
        _executable_directory = std::move(path);
    }

    std::filesystem::path executable_name() {
        return _executable_name;
    }

    std::filesystem::path executable_directory() {
        return _executable_directory;
    }

    std::filesystem::path assets_directory() {
        return utils::concat_paths(_executable_directory, "assets");
    }

    std::filesystem::path shaders_directory() {
        // TODO a different path
        return utils::concat_paths(_executable_directory, "shaders");
    }

#if ANTON_WITH_EDITOR
    static std::filesystem::path _project_directory;

    void set_project_directory(std::filesystem::path path) {
        _project_directory = std::move(path);
    }

    std::filesystem::path project_directory() {
        return _project_directory;
    }
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths
