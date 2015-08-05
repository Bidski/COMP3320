#version 150 core

in vec3 Colour;
in vec2 TextureCoord;

out vec4 outColour;

uniform sampler2D texKitten;
uniform sampler2D texPuppy;

void main()
{
    vec4 colKitten = texture(texKitten, TextureCoord);
    vec4 colPuppy  = texture(texPuppy,  TextureCoord);
    vec4 texColour = mix(colKitten, colPuppy, 0.5);
    outColour = vec4(Colour, 1.0) * texColour;
}

