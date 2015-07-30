#version 150 core

uniform vec3 triangleColour;

out vec4 outColour;

void main()
{
    outColour = vec4(triangleColour, 1.0);
}

