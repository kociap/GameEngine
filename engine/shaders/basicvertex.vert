#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec3 in_bitangent;
layout(location = 4) in vec2 tex_coordinates;
layout(location = 5) in uint draw_id;

uniform mat4 projection;
uniform mat4 view;

layout (std140, binding = 1) readonly buffer Matrices {
    mat4 model_matrices[];
};

out Frag_Data {
    mat3 tbn;
    vec3 normal;
    vec2 tex_coords;
    uint draw_id;
} vs_out;

void main() {
    const mat4 model = model_matrices[draw_id];
    const mat3 model_reduced = transpose(inverse(mat3(model)));
    const vec3 normal = normalize(model_reduced * in_normal);
    const vec3 tangent = normalize(model_reduced * in_tangent);
    const vec3 bitangent = normalize(model_reduced * in_bitangent);

    gl_Position = projection * view * model * vec4(pos, 1.0);
    vs_out.normal = normal;
    vs_out.tex_coords = tex_coordinates;
    vs_out.tbn = mat3(tangent, bitangent, normal);
    vs_out.draw_id = draw_id;
}
