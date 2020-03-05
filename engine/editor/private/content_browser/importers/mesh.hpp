#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE

#include <core/stl/vector.hpp>
#include <cstdint>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>

namespace anton_engine::importers {
    class Face {
    public:
        anton_stl::Vector<uint32_t> indices;
    };

    class Mesh {
    public:
        std::string name;
        anton_stl::Vector<Face> faces;
        anton_stl::Vector<Vector3> vertices;
        anton_stl::Vector<Vector3> normals;
        anton_stl::Vector<Vector3> texture_coordinates;
    };
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
