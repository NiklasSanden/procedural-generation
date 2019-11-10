#version 450 core
layout (location = 0) in vec2 aPos;

out vec3 FragPosWorld;
out vec3 FragPosView;
out vec3 Normal;
out vec3 WorldNormal;

uniform mat4 view;
uniform mat4 projection;
uniform float chunkLength;
uniform float cellLength;
uniform int pointsPerAxis;
uniform vec2 chunkPosition;

layout(binding = 0) uniform sampler2D NoiseTexture;

void main()
{
	vec4 position = vec4(chunkPosition.x * chunkLength + aPos.x * cellLength, 0.0, chunkPosition.y * chunkLength + aPos.y * cellLength, 1.0);
	
	ivec2 texelPoint = ivec2(round(vec2(aPos.x, aPos.y) + vec2((pointsPerAxis - 1.0) / 2.0 + 1, (pointsPerAxis - 1.0) / 2.0 + 1)));
	position.y = texelFetch(NoiseTexture, texelPoint, 0).r * 1.2;

	FragPosWorld = vec3(position);
	FragPosView = vec3(view * vec4(FragPosWorld, 1.0));

	position = projection * view * position;
	//position.z -= 0.2 * (3 - min(3, LOD));
    gl_Position = position;


	WorldNormal = vec3(0.0, 1.0, 0.0);
	Normal = vec3(view * vec4(WorldNormal, 0.0));
}