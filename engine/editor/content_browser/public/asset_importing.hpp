#ifndef ENGINE_ASSET_IMPORTER_HPP_INCLUDE
#define ENGINE_ASSET_IMPORTER_HPP_INCLUDE

#include <filesystem>

namespace anton_engine::asset_importing {
    void import(std::filesystem::path const& path);
}; // namespace anton_engine::asset_importing

#endif // !ENGINE_ASSET_IMPORTER_HPP_INCLUDE
