#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

varying vec2 center;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    center = uv;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
