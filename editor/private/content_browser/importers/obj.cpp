#include <content_browser/importers/obj.hpp>

#include <cctype>

namespace anton_engine::importers {
  bool test_obj(anton::String_View const file_extension,
                anton::Slice<u8 const> const)
  {
    if(file_extension == u8".obj") {
      return true;
    } else {
      // TODO: Look for keywords in the file.
      return false;
    }
  }

  using stream_iterator = anton::Array<u8>::const_iterator;

  struct Face_Internal {
    anton::Array<u32> vertex_indices;
    anton::Array<u32> texture_coordinate_indices;
    anton::Array<u32> normal_indices;
  };

  struct Mesh_Internal {
    anton::String name;
    anton::Array<Face_Internal> faces;
  };

  static void seek(stream_iterator& iter, char c)
  {
    while(*iter != c && *iter != '\0') {
      ++iter;
    }
  }

  template<typename P>
  void seek(stream_iterator& iter, P p)
  {
    while(!p(*iter)) {
      ++iter;
    }
  }

  static float read_float(stream_iterator& iter)
  {
    while(std::isspace(*iter) && *iter != '\n') {
      ++iter;
    }

    float sign = 1.0f;
    if(*iter == '-') {
      sign = -1.0f;
      ++iter;
    }

    float number = 0.0f;
    while(std::isdigit(*iter)) {
      number *= 10.0f;
      number += static_cast<float>(*iter - 48);
      ++iter;
    }

    if(*iter == '.') {
      ++iter;
    }

    float divisor = 1.0f;
    while(std::isdigit(*iter)) {
      divisor /= 10.0f;
      number += static_cast<float>(*iter - 48) * divisor;
      ++iter;
    }

    return sign * number;
  }

  static i64 read_int64(stream_iterator& iter)
  {
    while(std::isspace(*iter) && *iter != '\n') {
      ++iter;
    }

    i64 sign = 1;
    if(*iter == '-') {
      sign = -1;
      ++iter;
    }

    i64 number = 0;
    while(std::isdigit(*iter)) {
      number *= 10;
      number += static_cast<i64>(*iter - 48);
      ++iter;
    }

    return sign * number;
  }

  static void parse_obj(stream_iterator obj_it, stream_iterator obj_it_end,
                        anton::Array<Vec3>& vertices,
                        anton::Array<Vec3>& normals,
                        anton::Array<Vec3>& texture_coordinates,
                        anton::Array<Mesh_Internal>& meshes_internal)
  {
    // TODO add support for object groups (statement g)
    // TODO parse lines and points
    // TODO add more stream end checks to ensure we are not going past obj_it_end
    Mesh_Internal* current_mesh = nullptr;
    while(obj_it != obj_it_end) {
      char c1 = *obj_it++;
      char c2 = *obj_it++;
      if(c1 == 'v' && c2 == ' ') {
        // Geometric vertex
        // May have 4 (x, y, z, w) parameters if the objects is a rational curve or a surface
        //   We just skip w because we don't support it
        Vec3 vertex_postion;
        vertex_postion.x = read_float(obj_it);
        vertex_postion.y = read_float(obj_it);
        vertex_postion.z = read_float(obj_it);
        vertices.push_back(vertex_postion);
        seek(obj_it, '\n');
        ++obj_it;
      } else if(c1 == 'v' && c2 == 'n') {
        // Normal vector
        Vec3 vertex_normal;
        vertex_normal.x = read_float(obj_it);
        vertex_normal.y = read_float(obj_it);
        vertex_normal.z = read_float(obj_it);
        normals.push_back(vertex_normal);
        seek(obj_it, '\n');
        ++obj_it;
      } else if(c1 == 'v' && c2 == 't') {
        // Vertex texture coordinates
        Vec3 vertex_uv;
        vertex_uv.x = read_float(obj_it);
        vertex_uv.y = read_float(obj_it);
        vertex_uv.z = read_float(obj_it);
        texture_coordinates.push_back(vertex_uv);
        seek(obj_it, '\n');
        ++obj_it;
      } else if(c1 == 'f') {
        // Face
        // Note: reference numbers in obj may be negative (relative to current position)...
        Face_Internal face;
        while(true) {
          i64 pos_index = read_int64(obj_it);
          if(pos_index == 0) {
            break;
          } else {
            if(pos_index < 0) {
              pos_index = static_cast<i64>(vertices.size()) + pos_index;
            } else {
              pos_index -= 1; // OBJ uses 1 based arrays, thus subtract 1
            }
            face.vertex_indices.push_back(pos_index);
          }

          if(*obj_it == '/') {
            ++obj_it;
            i64 uv_index = read_int64(obj_it);
            if(uv_index != 0) {
              if(uv_index < 0) {
                uv_index =
                  static_cast<i64>(texture_coordinates.size()) + uv_index;
              } else {
                uv_index -= 1; // OBJ uses 1 based arrays, thus subtract 1
              }
              face.texture_coordinate_indices.push_back(uv_index);
            }
          }

          if(*obj_it == '/') {
            ++obj_it;
            i64 normal_index = read_int64(obj_it);
            if(normal_index < 0) {
              normal_index = static_cast<i64>(normals.size()) + normal_index;
            } else {
              normal_index -= 1; // OBJ uses 1 based arrays, thus subtract 1
            }
            face.normal_indices.push_back(normal_index);
          }
        }
        current_mesh->faces.push_back(ANTON_MOV(face));
        seek(obj_it, '\n');
        ++obj_it;
      } else if(c1 == 'o') {
        // Object name
        while(std::isspace(*obj_it) && *obj_it != '\n') {
          ++obj_it;
        }

        if(*obj_it == '\n') {
          // Could not find an object's name
          // throw (what exception?)
        } else {
          auto begin = obj_it;
          while(!std::isspace(*obj_it)) {
            ++obj_it;
          }
          meshes_internal.emplace_back();
          current_mesh = &meshes_internal[meshes_internal.size() - 1];
          current_mesh->name = anton::String((char8*)begin, obj_it - begin);
          seek(obj_it, '\n');
          ++obj_it;
        }
      } else {
        // Skip comments and unsupported or unknown statements/attributes
        seek(obj_it, '\n');
        ++obj_it;
      }
    }
  }

  anton::Array<Mesh> import_obj(anton::Array<u8> const& obj_data)
  {
    // TODO face triangulation
    anton::Array<Vec3> vertices;
    anton::Array<Vec3> normals;
    anton::Array<Vec3> texture_coordinates;
    anton::Array<Mesh_Internal> meshes_internal;
    parse_obj(obj_data.begin(), obj_data.end(), vertices, normals,
              texture_coordinates, meshes_internal);

    anton::Array<Mesh> meshes(anton::reserve, meshes_internal.size());
    for(Mesh_Internal const& mesh_internal: meshes_internal) {
      Mesh mesh;
      mesh.name = mesh_internal.name;
      anton::Array<Face> faces(anton::reserve, mesh_internal.faces.size());
      for(Face_Internal const& face_internal: mesh_internal.faces) {
        Face face;
        for(u32 const index: face_internal.vertex_indices) {
          mesh.vertices.push_back(vertices[index]);
          u32 new_index = mesh.vertices.size() - 1;
          face.indices.push_back(new_index);
        }

        if(face_internal.normal_indices.size() != 0) {
          for(u32 const index: face_internal.normal_indices) {
            mesh.normals.push_back(normals[index]);
          }
        } else {
          // TODO compute normals (or not)
        }

        if(face_internal.texture_coordinate_indices.size() != 0) {
          for(u32 const index: face_internal.texture_coordinate_indices) {
            mesh.texture_coordinates.push_back(texture_coordinates[index]);
          }
        }

        faces.push_back(ANTON_MOV(face));
      }
      mesh.faces = ANTON_MOV(faces);
      meshes.push_back(ANTON_MOV(mesh));
    }
    return meshes;
  }
} // namespace anton_engine::importers
