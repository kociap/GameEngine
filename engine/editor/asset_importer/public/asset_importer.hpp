#ifndef ENGINE_ASSET_IMPORTER_HPP_INCLUDE
#define ENGINE_ASSET_IMPORTER_HPP_INCLUDE

#include <filesystem>

class Asset_Importer {
public:
    Asset_Importer(std::filesystem::path output_directory);

    void import_image(std::filesystem::path const& image_path);

private:
    std::filesystem::path output_directory;
};

#endif // !ENGINE_ASSET_IMPORTER_HPP_INCLUDE
