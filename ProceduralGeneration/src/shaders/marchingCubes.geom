#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 12) out;


out vec3 FragPosView;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

void main() {
	
	//FragPosView = vec3(view * model * vec4(aPos, 1.0));
	//Normal = normal * aNormal;
}
