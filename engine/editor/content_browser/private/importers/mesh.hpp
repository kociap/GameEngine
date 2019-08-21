#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <cstdint>
#include <math/vector2.hpp>
#include <math/vector3.hpp>

namespace importers {
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
} // namespace importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_MESH_HPP_INCLUDE
