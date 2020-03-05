#include <content_browser/importers/obj.hpp>

#include <cctype>

namespace anton_engine::importers {
    bool test_obj(anton_stl::String_View const file_extension, anton_stl::Slice<u8 const> const) {
        if (file_extension == u8".obj") {
            return true;
        } else {
            // TODO: Look for keywords in the file.
            return false;
        }
    }

    using stream_iterator = anton_stl::Vector<uint8_t>::const_iterator;

    struct Face_Internal {
        anton_stl::Vector<uint32_t> vertex_indices;
        anton_stl::Vector<uint32_t> texture_coordinate_indices;
        anton_stl::Vector<uint32_t> normal_indices;
    };

    struct Mesh_Internal {
        std::string name;
        anton_stl::Vector<Face_Internal> faces;
    };

    static void seek(stream_iterator& iter, char c) {
        while (*iter != c && *iter != '\0') {
            ++iter;
        }
    }

    template <typename P>
    void seek(stream_iterator& iter, P p) {
        while (!p(*iter)) {
            ++iter;
        }
    }

    static float read_float(stream_iterator& iter) {
        while (std::isspace(*iter) && *iter != '\n') {
            ++iter;
        }

        float sign = 1.0f;
        if (*iter == '-') {
            sign = -1.0f;
            ++iter;
        }

        float number = 0.0f;
        while (std::isdigit(*iter)) {
            number *= 10.0f;
            number += static_cast<float>(*iter - 48);
            ++iter;
        }

        if (*iter == '.') {
            ++iter;
        }

        float divisor = 1.0f;
        while (std::isdigit(*iter)) {
            divisor /= 10.0f;
            number += static_cast<float>(*iter - 48) * divisor;
            ++iter;
        }

        return sign * number;
    }

    static int64_t read_int64(stream_iterator& iter) {
        while (std::isspace(*iter) && *iter != '\n') {
            ++iter;
        }

        int64_t sign = 1;
        if (*iter == '-') {
            sign = -1;
            ++iter;
        }

        int64_t number = 0;
        while (std::isdigit(*iter)) {
            number *= 10;
            number += static_cast<int64_t>(*iter - 48);
            ++iter;
        }

        return sign * number;
    }

    static void parse_obj(stream_iterator obj_it, stream_iterator obj_it_end, anton_stl::Vector<Vector3>& vertices, anton_stl::Vector<Vector3>& normals,
                          anton_stl::Vector<Vector3>& texture_coordinates, anton_stl::Vector<Mesh_Internal>& meshes_internal) {
        // TODO add support for object groups (statement g)
        // TODO parse lines and points
        // TODO add more stream end checks to ensure we are not going past obj_it_end
        Mesh_Internal* current_mesh = nullptr;
        while (obj_it != obj_it_end) {
            char c1 = *obj_it++;
            char c2 = *obj_it++;
            if (c1 == 'v' && c2 == ' ') {
                // Geometric vertex
                // May have 4 (x, y, z, w) parameters if the objects is a rational curve or a surface
                //   We just skip w because we don't support it
                Vector3 vertex_postion;
                vertex_postion.x = read_float(obj_it);
                vertex_postion.y = read_float(obj_it);
                vertex_postion.z = read_float(obj_it);
                vertices.push_back(vertex_postion);
                seek(obj_it, '\n');
                ++obj_it;
            } else if (c1 == 'v' && c2 == 'n') {
                // Normal vector
                Vector3 vertex_normal;
                vertex_normal.x = read_float(obj_it);
                vertex_normal.y = read_float(obj_it);
                vertex_normal.z = read_float(obj_it);
                normals.push_back(vertex_normal);
                seek(obj_it, '\n');
                ++obj_it;
            } else if (c1 == 'v' && c2 == 't') {
                // Vertex texture coordinates
                Vector3 vertex_uv;
                vertex_uv.x = read_float(obj_it);
                vertex_uv.y = read_float(obj_it);
                vertex_uv.z = read_float(obj_it);
                texture_coordinates.push_back(vertex_uv);
                seek(obj_it, '\n');
                ++obj_it;
            } else if (c1 == 'f') {
                // Face
                // Note: reference numbers in obj may be negative (relative to current position)...
                Face_Internal face;
                while (true) {
                    int64_t pos_index = read_int64(obj_it);
                    if (pos_index == 0) {
                        break;
                    } else {
                        if (pos_index < 0) {
                            pos_index = static_cast<int64_t>(vertices.size()) + pos_index;
                        } else {
                            pos_index -= 1; // OBJ uses 1 based arrays, thus subtract 1
                        }
                        face.vertex_indices.push_back(pos_index);
                    }

                    if (*obj_it == '/') {
                        ++obj_it;
                        int64_t uv_index = read_int64(obj_it);
                        if (uv_index != 0) {
                            if (uv_index < 0) {
                                uv_index = static_cast<int64_t>(texture_coordinates.size()) + uv_index;
                            } else {
                                uv_index -= 1; // OBJ uses 1 based arrays, thus subtract 1
                            }
                            face.texture_coordinate_indices.push_back(uv_index);
                        }
                    }

                    if (*obj_it == '/') {
                        ++obj_it;
                        int64_t normal_index = read_int64(obj_it);
                        if (normal_index < 0) {
                            normal_index = static_cast<int64_t>(normals.size()) + normal_index;
                        } else {
                            normal_index -= 1; // OBJ uses 1 based arrays, thus subtract 1
                        }
                        face.normal_indices.push_back(normal_index);
                    }
                }
                current_mesh->faces.push_back(std::move(face));
                seek(obj_it, '\n');
                ++obj_it;
            } else if (c1 == 'o') {
                // Object name
                while (std::isspace(*obj_it) && *obj_it != '\n') {
                    ++obj_it;
                }

                if (*obj_it == '\n') {
                    // Could not find an object's name
                    // throw (what exception?)
                } else {
                    auto begin = obj_it;
                    while (!std::isspace(*obj_it)) {
                        ++obj_it;
                    }
                    meshes_internal.emplace_back();
                    current_mesh = &meshes_internal[meshes_internal.size() - 1];
                    current_mesh->name = std::string(begin, obj_it);
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

    anton_stl::Vector<Mesh> import_obj(anton_stl::Vector<uint8_t> const& obj_data) {
        // TODO face triangulation
        anton_stl::Vector<Vector3> vertices;
        anton_stl::Vector<Vector3> normals;
        anton_stl::Vector<Vector3> texture_coordinates;
        anton_stl::Vector<Mesh_Internal> meshes_internal;
        parse_obj(obj_data.begin(), obj_data.end(), vertices, normals, texture_coordinates, meshes_internal);

        anton_stl::Vector<Mesh> meshes(anton_stl::reserve, meshes_internal.size());
        for (Mesh_Internal const& mesh_internal: meshes_internal) {
            Mesh mesh;
            mesh.name = std::move(mesh_internal.name);
            anton_stl::Vector<Face> faces(anton_stl::reserve, mesh_internal.faces.size());
            for (Face_Internal const& face_internal: mesh_internal.faces) {
                Face face;
                for (uint32_t const index: face_internal.vertex_indices) {
                    mesh.vertices.push_back(vertices[index]);
                    uint32_t new_index = mesh.vertices.size() - 1;
                    face.indices.push_back(new_index);
                }

                if (face_internal.normal_indices.size() != 0) {
                    for (uint32_t const index: face_internal.normal_indices) {
                        mesh.normals.push_back(normals[index]);
                    }
                } else {
                    // TODO compute normals (or not)
                }

                if (face_internal.texture_coordinate_indices.size() != 0) {
                    for (uint32_t const index: face_internal.texture_coordinate_indices) {
                        mesh.texture_coordinates.push_back(texture_coordinates[index]);
                    }
                }

                faces.push_back(std::move(face));
            }
            mesh.faces = std::move(faces);
            meshes.push_back(std::move(mesh));
        }
        return meshes;
    }
} // namespace anton_engine::importers
