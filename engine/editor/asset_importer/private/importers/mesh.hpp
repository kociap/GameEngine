#ifndef EDITOR_IMPORTERS_MESH_HPP_INCLUDE
#define EDITOR_IMPORTERS_MESH_HPP_INCLUDE

#include <containers/vector.hpp>
#include <cstdint>
#include <math/vector2.hpp>
#include <math/vector3.hpp>

namespace importers {
    class Face {
    public:
        containers::Vector<uint32_t> indices;
    };

    class Mesh {
    public:
        std::string name;
        containers::Vector<Face> faces;
        containers::Vector<Vector3> vertices;
        containers::Vector<Vector3> normals;
        containers::Vector<Vector3> texture_coordinates;
    };
} // namespace importers

#endif // !EDITOR_IMPORTERS_MESH_HPP_INCLUDE
