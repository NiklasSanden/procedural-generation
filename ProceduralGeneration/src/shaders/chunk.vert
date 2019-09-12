#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPosView;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

	FragPosView = vec3(view * model * vec4(aPos, 1.0));
	Normal = normal * aNormal;
}