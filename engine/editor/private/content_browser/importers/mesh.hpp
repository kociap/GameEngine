#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE

#include <core/atl/vector.hpp>
#include <core/types.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>
#include <core/atl/string.hpp>

namespace anton_engine::importers {
    class Face {
    public:
        atl::Vector<u32> indices;
    };

    class Mesh {
    public:
        atl::String name;
        atl::Vector<Face> faces;
        atl::Vector<Vector3> vertices;
        atl::Vector<Vector3> normals;
        atl::Vector<Vector3> texture_coordinates;
    };
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
