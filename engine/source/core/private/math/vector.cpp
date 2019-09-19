#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <math/vector4.hpp>

namespace anton_engine {
    Vector2::Vector2(Vector3 const& vec): x(vec.x), y(vec.y) {}
    Vector2::Vector2(Vector4 const& vec): x(vec.x), y(vec.y) {}

    Vector3::Vector3(Vector2 const& vec, float z /* = 0 */): x(vec.x), y(vec.y), z(z) {}
    Vector3::Vector3(Vector4 const& vec): x(vec.x), y(vec.y), z(vec.z) {}

    Vector4::Vector4(Vector2 const& vec, float z /* = 0 */, float w /* = 0 */): x(vec.x), y(vec.y), z(z), w(w) {}
    Vector4::Vector4(Vector3 const& vec, float w /* = 0 */): x(vec.x), y(vec.y), z(vec.z), w(w) {}
} // namespace anton_engine
