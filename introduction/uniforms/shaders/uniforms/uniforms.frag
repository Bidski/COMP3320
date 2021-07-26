#version 330 core

// Fragment shader output
layout(location = 0) out vec4 FragColor;

// Set in OpenGL code
uniform vec4 frag_colour;

void main() {
    FragColor = frag_colour;
}
