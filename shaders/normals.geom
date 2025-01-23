#version 450 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in Vert_data {
    vec3 normal;
}
vert_data[];

uniform float normal_magnitude;

void generate_line(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(vert_data[index].normal, 0.0) * normal_magnitude;
    EmitVertex();
    EndPrimitive();
}

void main() {
    generate_line(0);
    generate_line(1);
    generate_line(2);
}