#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE

#include <core/atl/vector.hpp>
#include <cstdint>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>

namespace anton_engine::importers {
    class Face {
    public:
        atl::Vector<uint32_t> indices;
    };

    class Mesh {
    public:
        std::string name;
        atl::Vector<Face> faces;
        atl::Vector<Vector3> vertices;
        atl::Vector<Vector3> normals;
        atl::Vector<Vector3> texture_coordinates;
    };
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
