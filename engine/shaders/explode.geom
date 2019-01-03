#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform float time_factor;

in Frag_data {
    vec3 normal;
    vec3 fragment_position;
    vec2 texture_coordinates;
}
frag_data[];

out Frag_data {
    vec3 normal;
    vec3 fragment_position;
    vec2 texture_coordinates;
}
fragment_data;

vec3 compute_normal() {
    vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
    return position + vec4(normal, 0.0) * ((sin(time_factor) + 1.0) / 2.0);
}

void main() {
    vec3 triangle_normal = compute_normal();

    fragment_data.normal = frag_data[0].normal;
    fragment_data.texture_coordinates = frag_data[0].texture_coordinates;
    fragment_data.fragment_position = frag_data[0].fragment_position + frag_data[0].normal * ((sin(time_factor) + 1.0) / 2.0);
    gl_Position = explode(gl_in[0].gl_Position, triangle_normal);
    EmitVertex();

    fragment_data.normal = frag_data[1].normal;
    fragment_data.texture_coordinates = frag_data[1].texture_coordinates;
    fragment_data.fragment_position = frag_data[1].fragment_position + frag_data[1].normal * ((sin(time_factor) + 1.0) / 2.0);
    gl_Position = explode(gl_in[1].gl_Position, triangle_normal);
    EmitVertex();

    fragment_data.normal = frag_data[2].normal;
    fragment_data.texture_coordinates = frag_data[2].texture_coordinates;
    fragment_data.fragment_position = frag_data[2].fragment_position + frag_data[2].normal * ((sin(time_factor) + 1.0) / 2.0);
    gl_Position = explode(gl_in[2].gl_Position, triangle_normal);
    EmitVertex();

    EndPrimitive();
}