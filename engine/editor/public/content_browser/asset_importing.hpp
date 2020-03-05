#ifndef ENGINE_ASSET_IMPORTING_HPP_INCLUDE
#define ENGINE_ASSET_IMPORTING_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/slice.hpp>
#include <core/stl/string_view.hpp>
#include <core/stl/vector.hpp>
#include <filesystem>
#include <engine/mesh.hpp>

namespace anton_engine::asset_importing {
    class Imported_Meshes {
    public:
        anton_stl::Vector<i32> hierarchy;
        anton_stl::Vector<Mesh> meshes;
    };

    void import_image(std::filesystem::path const&);
    Imported_Meshes import_mesh(std::filesystem::path const&);

    void save_meshes(anton_stl::String_View filename, anton_stl::Slice<u64 const> guids, anton_stl::Slice<Mesh const> meshes);
}; // namespace anton_engine::asset_importing

#endif // !ENGINE_ASSET_IMPORTING_HPP_INCLUDE
