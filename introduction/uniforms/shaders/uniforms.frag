#version 330 core

// Fragment shader output
out vec4 FragColor;

// Set in OpenGL code
uniform vec4 ourColour;

void main() {
    FragColor = ourColour;
}
