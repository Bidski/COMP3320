#version 150 core

in vec2 position;
in vec2 textureCoord;
in vec3 colour;

out vec3 Colour;
out vec2 TextureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    Colour = colour;
    TextureCoord = textureCoord;
    gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
}

