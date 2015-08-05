#version 150 core

in vec3 position;
in vec2 textureCoord;
in vec3 colour;

out vec3 Colour;
out vec2 TextureCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    Colour = colour;
    TextureCoord = textureCoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}

