#include <paths.hpp>
#include <paths_internal.hpp>
#include <utils/filesystem.hpp>

namespace anton_engine::paths {
    static std::filesystem::path _engine_executable_name;
    static std::filesystem::path _engine_executable_directory;
    static std::filesystem::path _project_directory;

    void set_engine_executable_name(std::filesystem::path name) {
        _engine_executable_name = std::move(name);
    }

    void set_engine_executable_directory(std::filesystem::path path) {
        _engine_executable_directory = std::move(path);
    }

    void set_project_directory(std::filesystem::path path) {
        _project_directory = std::move(path);
    }

    std::filesystem::path engine_executable_name() {
        return _engine_executable_name;
    }

    std::filesystem::path engine_executable_directory() {
        return _engine_executable_directory;
    }

    std::filesystem::path project_directory() {
        return _project_directory;
    }

#if !GE_BUILD_SHIPPING
    std::filesystem::path assets_directory() {
        return utils::concat_paths(_project_directory, "assets");
    }

    std::filesystem::path shaders_directory() {
        // TODO a different path
        return utils::concat_paths(_engine_executable_directory, "shaders");
    }
#endif
} // namespace anton_engine::paths
