#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 rotation_axis;
layout(location = 2) in float scale_factor;

uniform mat4 model_mat;
uniform mat4 vp_mat;
uniform vec3 camera_pos;
uniform float line_width;

void main() {
    mat3 model_inv = inverse(transpose(mat3(model_mat)));
    vec3 axis_world = normalize(model_inv * rotation_axis);
    vec3 pos_world = vec3(model_mat * vec4(pos, 1));
    vec3 projected_camera = camera_pos - axis_world * dot(axis_world, camera_pos - pos_world);

    vec3 view_vec_projected = projected_camera - pos_world;
    float len = length(view_vec_projected);
    if(len > 0.00001) {
        vec3 normalized_view = view_vec_projected / len;
        float expand_dir = (gl_VertexID & 1) == 1 ? -1 : 1;
        gl_Position = vp_mat * vec4(pos_world + scale_factor * line_width * expand_dir * cross(normalized_view, axis_world), 1);
    } else {
        gl_Position = vp_mat * vec4(pos_world, 1);
    }
}
