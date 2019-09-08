#version 450 core
out vec4 FragColour;

in vec2 texCoord;

// textures
uniform sampler2D texture0;

void main()
{
	FragColour = texture(texture0, texCoord);
}