#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPosWorld;
out vec3 FragPosView;
out vec3 Normal;
out vec3 WorldNormal;

uniform mat4 view;
uniform mat4 projection;
uniform int LOD;

void main()
{
	vec4 position = projection * view * vec4(aPos, 1.0);
	position.z -= 0.2 * (3 - min(3, LOD));
    gl_Position = position;


	FragPosWorld = aPos;
	FragPosView = vec3(view * vec4(aPos, 1.0));
	Normal = vec3(view * vec4(aNormal, 0.0));
	WorldNormal = aNormal;
}