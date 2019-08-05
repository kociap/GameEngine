#ifndef ENGINE_ASSET_IMPORTER_HPP_INCLUDE
#define ENGINE_ASSET_IMPORTER_HPP_INCLUDE

#include <filesystem>

namespace asset_importing {
    void import(std::filesystem::path const& path);
}; // namespace asset_importing

#endif // !ENGINE_ASSET_IMPORTER_HPP_INCLUDE
