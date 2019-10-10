#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPosView;
out vec3 Normal;
out vec3 WorldNormal;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);

	FragPosView = vec3(view * vec4(aPos, 1.0));
	Normal = vec3(view * vec4(aNormal, 0.0));
	WorldNormal = aNormal;
}