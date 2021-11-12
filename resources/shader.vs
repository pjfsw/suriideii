#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coord;
layout (location = 2) in vec3 normal;

uniform mat4 gPerspective;
uniform mat4 gWorld;
uniform mat4 gCamera;

out vec2 tex_coord_0;
out vec4 frag_color_0;

struct Light {
    vec3 direction;
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
    float specular_intensity;
    float specular_power;
};
uniform Light gLight;

void main() {
    vec4 view_position = gCamera * gWorld * vec4(position, 1.0);
    gl_Position = gPerspective * view_position;
    tex_coord_0 = tex_coord;
    vec3 normal_0 = normalize((gWorld * vec4(normal,0.0)).xyz);

    // Diffuse lighting component
    float diffuse_factor = dot(normalize(normal_0), -gLight.direction); 
    float diffuse_intensity = diffuse_factor * gLight.diffuse_intensity;

    // Specular lighting component
    float specular_intensity = gLight.specular_intensity * gLight.specular_power / 100;

    frag_color_0 = vec4(gLight.color, 1) * (specular_intensity + diffuse_intensity + gLight.ambient_intensity);
}
