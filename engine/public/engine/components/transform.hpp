#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/quat.hpp>
#include <anton/math/transform.hpp>
#include <anton/math/vec3.hpp>
#include <core/class_macros.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
  class COMPONENT Transform {
  public:
    Quat local_rotation;
    Vec3 local_position;
    Vec3 local_scale = Vec3{1.0f, 1.0f, 1.0f};

    void translate(Vec3 const& translation_vec)
    {
      local_position += translation_vec;
    }

    void scale(Vec3 const& scale_vec)
    {
      local_scale *= scale_vec;
    }

    // axis to rotate about
    // angle in radians
    void rotate(Vec3 axis, float angle)
    {
      float half_angle_sin = math::sin(angle / 2);
      Quat q(axis.x * half_angle_sin, axis.y * half_angle_sin,
             axis.z * half_angle_sin, math::cos(angle / 2));
      local_rotation = q * local_rotation;
    }

    Mat4 to_matrix() const
    {
      Mat4 mat = math::scale(local_scale) * math::rotate(local_rotation) *
                 math::translate(local_position);
      return mat;
    }
  };

  inline Mat4 to_matrix(Transform const t)
  {
    Mat4 mat = math::scale(t.local_scale) * math::rotate(t.local_rotation) *
               math::translate(t.local_position);
    return mat;
  }
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Transform)
