#version 450 core
out vec4 FragColour;

//in vec2 texCoord;

// textures
//uniform sampler2D texture0;
//uniform sampler2D texture1;

void main()
{
    //FragColour = mix(texture(texture0, texCoord), texture(texture1, vec2(1 - texCoord.x, texCoord.y)), 0.35);
	FragColour = vec4(1.0, 0.0, 0.0, 1.0);
}