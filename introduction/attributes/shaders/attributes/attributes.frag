#version 330 core

// Fragment shader output
layout(location = 0) out vec4 FragColor;

// Colour from the vertex shader
in vec3 ourColour;

void main() {
    FragColor = vec4(ourColour, 1.0f);
}
