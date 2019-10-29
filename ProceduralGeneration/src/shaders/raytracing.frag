#version 450 core
out vec4 FragColour;

uniform sampler2D renderedTexture;

in vec2 TextureCoord;

void main()
{
    FragColour = texture(renderedTexture, TextureCoord);
}
