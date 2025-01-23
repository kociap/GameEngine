#version 450 core
layout(triangles) in;
layout(line_strip, max_vertices = 256) out;

in VS_Data {
    mat3 tbn;
    vec3 normal_clip;
    vec3 tangent_clip;
    vec3 bitangent_clip;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 tex_coords;
} vs_data[];

out Frag_Data {
    vec3 color;
} frag_data;

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D normal_map;
    float shininess;
    bool normal_map_attached;
};

uniform Material material;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float line_length = 0.5;

void emit_line(vec4 pos, vec4 direction, float dir_length, vec3 color) {
    gl_Position = pos;
    frag_data.color = color;
    EmitVertex();
    gl_Position = pos + direction * dir_length;
    frag_data.color = color;
    EmitVertex();
    EndPrimitive();
}

void emit_vert(vec4 pos, vec3 color) {
    gl_Position = pos;
    gl_PointSize = 15;
    frag_data.color = color;
    EmitVertex();
}

vec3 color_to_normal(vec3 c) {
    return c * 2.0 - 1.0;
}

const int normal_samples = 1;
const int normal_increment = 1;

void main() {
    vec3 normal = normalize(vs_data[0].normal_clip + vs_data[1].normal_clip + vs_data[2].normal_clip);
    vec3 tangent = normalize(vs_data[0].tangent_clip + vs_data[1].tangent_clip + vs_data[2].tangent_clip);
    vec3 bitangent = normalize(vs_data[0].bitangent_clip + vs_data[1].bitangent_clip + vs_data[2].bitangent_clip);
    vec4 mesh_offset = vec4(vs_data[0].normal_clip * 0.05, 0.0);
    for(int i = 1; i <= normal_samples * normal_increment; i += normal_increment) {
        for(int j = 1; j <= normal_samples * normal_increment; j += normal_increment) {
            for(int k = 1; k <= normal_samples * normal_increment; k += normal_increment) {
                vec4 triangle_center = (i * gl_in[0].gl_Position + j * gl_in[1].gl_Position + k * gl_in[2].gl_Position) / (i + j + k);
                vec2 tex_coords = (i * vs_data[0].tex_coords + j * vs_data[1].tex_coords + k * vs_data[2].tex_coords) / (i + j + k);

                vec3 tex_normal_sampled = texture(material.normal_map, tex_coords).rgb;
                vec3 tex_normal = color_to_normal(tex_normal_sampled);

                // vec4 tbned_normal = vec4(normalize(vs_data[0].tbn * tex_normal + vs_data[1].tbn * tex_normal + vs_data[2].tbn * tex_normal), 0.0);
                vec4 tbned_normal = vec4(normalize(vs_data[0].tbn * tex_normal), 0.0);
                vec3 color = vec3(abs(tbned_normal.x), abs(tbned_normal.y), abs(tbned_normal.z));
                // color = tex_normal;

                emit_line(triangle_center + mesh_offset, normalize(projection * view * tbned_normal), line_length, color);
            }
        }
    }
    // emit_line(triangle_center + mesh_offset, vec4(normal, 0.0), line_length, vec3(0, 0.35, 1));
    // emit_line(triangle_center + mesh_offset, vec4(tangent, 0.0), line_length, vec3(1, 0, 0));
    // emit_line(triangle_center + mesh_offset, vec4(bitangent, 0.0), line_length, vec3(0, 1, 0));
}