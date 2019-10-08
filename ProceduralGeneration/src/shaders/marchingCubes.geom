#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

out vec3 FragPosView;
out vec3 Normal;

in VS_OUT{
	vec3 chunkPosition;
	vec3 vertexPositions[8];
	float noiseValues[8];
} gs_in[];

uniform float cellLength;
uniform sampler2D triTable;

const int edgeTable[12][2]   = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 } };

// no model matrix necessary
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

vec3 lerpVector(vec3 a, vec3 b, float aValue, float bValue, float surfaceLevel);

void main() {
	const float surfaceLevel = 0.6;

	vec3 chunkPosition = gs_in[0].chunkPosition;
	vec3 cellPosition = vec3(gl_in[0].gl_Position);

	int cubeIndex = 0;
	for (int i = 0; i < 8; i++) {
		if (gs_in[0].noiseValues[i] > surfaceLevel) {
			cubeIndex |= 1 << i;
		}
	}


	for (int i = 0; i < 16; i += 3) {
//		if (triTable[cubeIndex * 16 + i] == -1) break;
//
//		int index1A = edgeTable[triTable[cubeIndex * 16 + i + 0]][0];
//		int index1B = edgeTable[triTable[cubeIndex * 16 + i + 0]][1];
//		vec3 tempVertexPosA = lerpVector(gs_in[0].vertexPositions[index1A], gs_in[0].vertexPositions[index1B], gs_in[0].noiseValues[index1A], gs_in[0].noiseValues[index1B], surfaceLevel);
//
//		int index2A = edgeTable[triTable[cubeIndex * 16 + i + 1]][0];
//		int index2B = edgeTable[triTable[cubeIndex * 16 + i + 1]][1];
//		vec3 tempVertexPosB = lerpVector(gs_in[0].vertexPositions[index2A], gs_in[0].vertexPositions[index2B], gs_in[0].noiseValues[index2A], gs_in[0].noiseValues[index2B], surfaceLevel);
//
//		int index3A = edgeTable[triTable[cubeIndex * 16 + i + 2]][0];
//		int index3B = edgeTable[triTable[cubeIndex * 16 + i + 2]][1];
//		vec3 tempVertexPosC = lerpVector(gs_in[0].vertexPositions[index3A], gs_in[0].vertexPositions[index3B], gs_in[0].noiseValues[index3A], gs_in[0].noiseValues[index3B], surfaceLevel);
//
//		vec3 AB = tempVertexPosB - tempVertexPosA;
//		vec3 AC = tempVertexPosC - tempVertexPosA;
//		Normal.x = AB.y * AC.z - AB.z * AC.y;
//		Normal.y = AB.z * AC.x - AB.x * AC.z;
//		Normal.z = AB.x * AC.y - AB.y * AC.x;
//		Normal = vec3(view * vec4(Normal, 0.0));
//
//		gl_Position = projection * view * vec4(chunkPosition + cellPosition + tempVertexPosA, 1.0);
//		FragPosView = vec3(view * vec4(chunkPosition + cellPosition + tempVertexPosA, 1.0));
//		EmitVertex();
//
//		gl_Position = projection * view * vec4(chunkPosition + cellPosition + tempVertexPosB, 1.0);
//		FragPosView = vec3(view * vec4(chunkPosition + cellPosition + tempVertexPosB, 1.0));
//		EmitVertex();
//
//		gl_Position = projection * view * vec4(chunkPosition + cellPosition + tempVertexPosC, 1.0);
//		FragPosView = vec3(view * vec4(chunkPosition + cellPosition + tempVertexPosC, 1.0));
//		EmitVertex();
//
//		EndPrimitive();
	}
}

vec3 lerpVector(vec3 a, vec3 b, float aValue, float bValue, float surfaceLevel) {
	return a + ((surfaceLevel - aValue) / (bValue - aValue)) * (b - a);
}
