#version 450 core

in vec3 normal;
in vec3 fragment_position;
in vec2 texture_coordinates;

struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular0;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    float shininess;
};

struct Camera {
    vec3 position;
    vec3 direction;
};

struct Point_light {
    vec3 position;
    vec3 color;

    float attentuation_constant;
    float attentuation_linear;
    float attentuation_quadratic;

    float diffuse_strength;
    float specular_strength;
};

struct Directional_light {
    vec3 direction;
    vec3 color;
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
};

#define point_lights_number 1

uniform Directional_light directional_light;
uniform Point_light point_lights[point_lights_number];
uniform Camera camera;
uniform Material material;
uniform vec3 view_position;

// vec3 compute_directional_light(Directional_light dir_light, vec3 normal, vec3 camera_position, vec3 fragment_position);
// vec3 compute_point_light(Point_light point_light, vec3 normal, vec3 camera_position, vec3 fragment_position);

out vec4 frag_color;

void main() {
    // vec3 lighting_result = vec3(0);
    // lighting_result += compute_directional_light(directional_light, normal, camera.position, fragment_position);
    // for (int i = 0; i < point_lights_number; ++i) {
    //     lighting_result += compute_point_light(point_lights[i], normal, camera.position, fragment_position);
    // }
    // frag_color = vec4(lighting_result, 1.0);

    frag_color = texture(material.texture_diffuse0, texture_coordinates);

    // vec3 ambient = vec3(texture(material.diffuse_map, texture_coordinates));
    // ambient = material.ambient_strength * ambient;

    // vec3 light_direction = normalize(light.position - fragment_position);
    // float angle = dot(light.direction, -light_direction);
    // if (light.blend_angle < angle) {
    //     float blend_strength = clamp((light.blend_angle - angle) / (light.blend_angle - light.cutoff_angle), 0.0, 1.0);

    //     float diffuse_strength = max(dot(normal, light_direction), 0.0);
    //     vec3 diffuse = diffuse_strength * vec3(texture(material.diffuse_map, texture_coordinates));

    //     vec3 view_direction = normalize(view_position - fragment_position);
    //     vec3 reflect_direction = reflect(-light_direction, normal);
    //     float specular_component = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
    //     vec3 specular = vec3(texture(material.specular_map, texture_coordinates)) * specular_component * light.color;

    //     float dist = length(light.position - fragment_position);
    //     float attentuation = 3.0 / (light.attentuation_constant + light.attentuation_linear * dist + light.attentuation_quadratic * dist * dist);

    //     diffuse = material.diffuse_strength * diffuse * attentuation * blend_strength;
    //     specular = material.specular_strength * specular * attentuation * blend_strength;
    //     frag_color = vec4(ambient + diffuse + specular, 1.0);
    // } else {
    //     frag_color = vec4(ambient, 1.0);
    // }
}

// vec3 compute_directional_light(Directional_light dir_light, vec3 normal, vec3 camera_position, vec3 fragment_position) {
//     vec3 frag_direction = normalize(camera_position - fragment_position);
//     vec3 texel_color = vec3(texture(material.diffuse_map, texture_coordinates));
//     vec3 ambient = dir_light.ambient_strength * texel_color;
//     vec3 diffuse = max(dot(normal, dir_light.direction), 0.0) * texel_color;

//     vec3 reflected_light_direction = reflect(-dir_light.direction, normal);
//     float specular_component = pow(max(dot(reflected_light_direction, frag_direction), 0.0), material.shininess);
//     vec3 specular = vec3(texture(material.specular_map, texture_coordinates)) * specular_component * dir_light.specular_strength;

//     return (ambient + diffuse + specular) * dir_light.color;
// }

// vec3 compute_point_light(Point_light point_light, vec3 normal, vec3 camera_position, vec3 fragment_position) {
//     vec3 light_frag_dir = normalize(point_light.position - fragment_position);
//     vec3 diffuse = max(dot(normal, light_frag_dir), 0.0) * vec3(texture(material.diffuse_map, texture_coordinates));
//     vec3 reflected_light_direction = reflect(-light_frag_dir, normal);
//     float specular_component = pow(max(dot(reflected_light_direction, normalize(camera_position - fragment_position)), 0.0), material.shininess);
//     vec3 specular = specular_component * vec3(texture(material.specular_map, texture_coordinates));

//     float dist = length(fragment_position - point_light.position);
//     float attentuation = 3.0 / (point_light.attentuation_constant + point_light.attentuation_linear * dist + point_light.attentuation_quadratic * dist * dist);

//     diffuse = diffuse * attentuation * point_light.diffuse_strength * point_light.color;
//     specular = specular * attentuation * point_light.specular_strength * point_light.color;
//     return diffuse + specular;
// }