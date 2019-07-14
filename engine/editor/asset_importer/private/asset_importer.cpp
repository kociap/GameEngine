#include <asset_importer.hpp>
#include <containers/vector.hpp>
#include <cstdint>
#include <importers/image.hpp>
#include <importers/mesh.hpp>
#include <importers/obj.hpp>
#include <importers/png.hpp>

#include <utils/filesystem.hpp>

#include <stdexcept>

Asset_Importer::Asset_Importer(std::filesystem::path output_dir): output_directory(std::move(output_dir)) {}

void Asset_Importer::import_image(std::filesystem::path const& image_path) {
    using namespace importers; // TODO I probably shouldn't do that
    containers::Vector<uint8_t> const image_raw = utils::read_file_binary(image_path);
    Image_Data decoded_image;
    if (test_png(image_raw)) {
        decoded_image = import_png(image_raw);
        // TODO convert from gamma encoded/srgb space to linear
        // TODO include necessary info in the output file
        // TODO meta files

        // TODO this is just temporary
        std::filesystem::path const out_file_name = utils::concat_paths(output_directory, utils::concat_paths(image_path.stem(), ".getex"));
        std::ofstream file(out_file_name, std::ios::binary | std::ios::trunc);
        file.write("GEtex", 5);
        file.write(reinterpret_cast<char*>(&decoded_image.width), 4);
        file.write(reinterpret_cast<char*>(&decoded_image.height), 4);
        file.write(reinterpret_cast<char*>(&decoded_image.pixel_format), sizeof(Image_Pixel_Format));
        file.write(reinterpret_cast<char*>(decoded_image.data.data()), decoded_image.data.size()); // TODO reinterpret_cast
    } else {
        throw std::runtime_error("Unsupported file format");
    }
}