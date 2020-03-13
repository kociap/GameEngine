#ifndef EDITOR_ASSET_IMPORTER_POSTPROCESS_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_POSTPROCESS_HPP_INCLUDE

#include <core/atl/slice.hpp>
#include <core/math/math.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>
#include <engine/mesh.hpp>

namespace anton_engine {
    inline void flip_texture_coordinates(atl::Slice<Vector3> const texture_coordinates) {
        for (Vector3& coords: texture_coordinates) {
            coords.y = 1.0f - coords.y;
        }
    }

    inline void flip_texture_coordinates(atl::Slice<Vector2> const texture_coordinates) {
        for (Vector2& coords: texture_coordinates) {
            coords.y = 1.0f - coords.y;
        }
    }

    inline void flip_texture_coordinates(atl::Slice<Vertex> const vertices) {
        for (Vertex& vertex: vertices) {
            vertex.uv_coordinates.y = 1.0f - vertex.uv_coordinates.y;
        }
    }

} // namespace anton_engine
#endif // !EDITOR_ASSET_IMPORTER_POSTPROCESS_HPP_INCLUDE
