#ifndef EDITOR_ASSET_IMPORTER_POSTPROCESS_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_POSTPROCESS_HPP_INCLUDE

#include <anton_stl/slice.hpp>
#include <math/math.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <mesh.hpp>

namespace anton_engine {
    inline void flip_texture_coordinates(anton_stl::Slice<Vector3> const texture_coordinates) {
        for (Vector3& coords: texture_coordinates) {
            coords.y = 1.0f - coords.y;
        }
    }

    inline void flip_texture_coordinates(anton_stl::Slice<Vector2> const texture_coordinates) {
        for (Vector2& coords: texture_coordinates) {
            coords.y = 1.0f - coords.y;
        }
    }

    inline void flip_texture_coordinates(anton_stl::Slice<Vertex> const vertices) {
        for (Vertex& vertex: vertices) {
            vertex.uv_coordinates.y = 1.0f - vertex.uv_coordinates.y;
        }
    }

} // namespace anton_engine
#endif // !EDITOR_ASSET_IMPORTER_POSTPROCESS_HPP_INCLUDE
