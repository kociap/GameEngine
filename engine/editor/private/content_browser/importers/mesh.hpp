#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE

#include <core/atl/array.hpp>
#include <core/atl/string.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>
#include <core/types.hpp>

namespace anton_engine::importers {
    class Face {
    public:
        atl::Array<u32> indices;
    };

    class Mesh {
    public:
        atl::String name;
        atl::Array<Face> faces;
        atl::Array<Vector3> vertices;
        atl::Array<Vector3> normals;
        atl::Array<Vector3> texture_coordinates;
    };
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
