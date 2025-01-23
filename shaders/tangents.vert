#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec3 in_bitangent;
layout(location = 4) in vec2 tex_coordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_Data {
    mat3 tbn;
    vec3 normal_clip;
    vec3 tangent_clip;
    vec3 bitangent_clip;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 tex_coords;
} vs_data;

void main() {
    mat4 vp_mat = projection * view;
    mat4 mvp_mat = vp_mat * model;
    mat3 model_reduced = mat3(model);
    mat3 model_normal_mat = transpose(inverse(model_reduced));
    vec3 normal = normalize(model_normal_mat * in_normal);
    vec3 tangent = normalize(model_reduced * in_tangent);
    vec3 bitangent = normalize(model_reduced * in_bitangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    gl_Position = mvp_mat * vec4(pos, 1.0);
    vs_data.normal = in_normal;
    vs_data.tangent = in_tangent;
    vs_data.bitangent = in_bitangent;
    vs_data.normal_clip = normalize(vec3(vp_mat * vec4(normal, 0.0)));
    vs_data.tangent_clip = normalize(vec3(vp_mat * vec4(tangent, 0.0)));
    vs_data.bitangent_clip = normalize(vec3(vp_mat * vec4(bitangent, 0.0)));
    // vs_data.bitangent_clip = normalize(vec3(vp_mat * vec4(cross(normal, tangent), 0.0)));
    vs_data.tbn = tbn;
    vs_data.tex_coords = tex_coordinates;
}
