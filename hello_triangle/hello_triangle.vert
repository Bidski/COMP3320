#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos.xyz, 1.0f);
}
