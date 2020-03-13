#ifndef ENGINE_ASSET_IMPORTING_HPP_INCLUDE
#define ENGINE_ASSET_IMPORTING_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/slice.hpp>
#include <core/atl/string_view.hpp>
#include <core/atl/vector.hpp>
#include <filesystem>
#include <engine/mesh.hpp>

namespace anton_engine::asset_importing {
    class Imported_Meshes {
    public:
        atl::Vector<i32> hierarchy;
        atl::Vector<Mesh> meshes;
    };

    void import_image(std::filesystem::path const&);
    Imported_Meshes import_mesh(std::filesystem::path const&);

    void save_meshes(atl::String_View filename, atl::Slice<u64 const> guids, atl::Slice<Mesh const> meshes);
}; // namespace anton_engine::asset_importing

#endif // !ENGINE_ASSET_IMPORTING_HPP_INCLUDE
