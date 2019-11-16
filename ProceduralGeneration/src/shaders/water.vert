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
uniform float waterLevel = -1.0;

layout(binding = 0) uniform sampler2D NoiseTexture;

void main()
{
	vec4 position = vec4(chunkPosition.x * chunkLength + aPos.x * cellLength, 0.0, chunkPosition.y * chunkLength + aPos.y * cellLength, 1.0);
	
	ivec2 texelPoint = ivec2(round(vec2(aPos.x, aPos.y) + vec2((pointsPerAxis - 1.0) / 2.0 + 1, (pointsPerAxis - 1.0) / 2.0 + 1)));
	position.y = texelFetch(NoiseTexture, texelPoint, 0).r * 1.4;

	position.y += waterLevel;

	FragPosWorld = vec3(position);
	FragPosView = vec3(view * vec4(FragPosWorld, 1.0));

	position = projection * view * position;
	//position.z -= 0.2 * (3 - min(3, LOD));
    gl_Position = position;


	float right = texelFetch(NoiseTexture, texelPoint + ivec2(1, 0), 0).r * 1.4;
	float left = texelFetch(NoiseTexture, texelPoint + ivec2(-1, 0), 0).r * 1.4;
	float up = texelFetch(NoiseTexture, texelPoint + ivec2(0, 1), 0).r * 1.4;
	float down = texelFetch(NoiseTexture, texelPoint + ivec2(0, -1), 0).r * 1.4;

	WorldNormal = vec3(0.0, 0.75, 0.0);
	WorldNormal.x = (left - right) / (2 * cellLength);
	WorldNormal.z = (down - up) / (2 * cellLength);
	WorldNormal = normalize(WorldNormal);

	Normal = vec3(view * vec4(WorldNormal, 0.0));
}