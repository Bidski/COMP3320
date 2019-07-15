#version 330 core

// Fragment shader output
out vec4 FragColor;

// Colour from the vertex shader
in vec3 ourColour;

void main() {
    FragColor = vec4(ourColour, 1.0f);
}
