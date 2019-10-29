#version 450 core
layout (location = 0) in vec2 aPos;

out vec2 TextureCoord;

void main()
{
	TextureCoord = aPos;
    gl_Position = vec4(aPos * 2.0 - vec2(1.0, 1.0), 0.0, 1.0);
}