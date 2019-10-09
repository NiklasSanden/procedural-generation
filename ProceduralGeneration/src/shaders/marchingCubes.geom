#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

out vec3 FragPosView;
out vec3 Normal;
out vec4 GeomColour;

in vec3 vertexPositions[][8];
in float noiseValues[][8];

uniform vec3 chunkPosition;
uniform float cellLength;
uniform usampler2D triTable;

const int edgeTable[12][2]   = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 } };

// no model matrix necessary
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

vec3 lerpVector(vec3 a, vec3 b, float aValue, float bValue, float surfaceLevel);

void main() {
	const float surfaceLevel = 0.6;

	vec3 cellPosition = vec3(gl_in[0].gl_Position);

//	gl_Position = projection * view * vec4(chunkPosition + cellPosition, 1.0);
//	FragPosView = vec3(view * vec4(chunkPosition + cellPosition, 1.0));
//	GeomColour = vec4(noiseValues[0][0], noiseValues[0][1], noiseValues[0][2], 1.0);
//	EmitVertex();
//
//	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vec3(1.0, 0.0, 0.0), 1.0);
//	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vec3(10, 0, 0), 1.0));
//	GeomColour = vec4(noiseValues[0][3], noiseValues[0][4], noiseValues[0][5], 1.0);
//	EmitVertex();
//
//	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vec3(0.0, 1.0, 0.0), 1.0);
//	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vec3(0, 10, 0), 1.0));
//	GeomColour = vec4(noiseValues[0][6], noiseValues[0][7], noiseValues[0][7], 1.0);
//	EmitVertex();
//
//	EndPrimitive();
//	return;

	int cubeIndex = 0;
	for (int i = 0; i < 8; i++) {
		if (noiseValues[0][i] > surfaceLevel) {
			cubeIndex |= 1 << i;
		}
	}


	for (int i = 0; i < 16; i += 3) {
		ivec2 texCoord = ivec2(cubeIndex, i);
		if (texelFetch(triTable, texCoord, 0).r == 32) break;

		int index1A = edgeTable[int(texelFetch(triTable, texCoord, 0).x)][0];
		int index1B = edgeTable[int(texelFetch(triTable, texCoord, 0).x)][1];
		vec3 tempVertexPosA = lerpVector(vertexPositions[0][index1A], vertexPositions[0][index1B], noiseValues[0][index1A], noiseValues[0][index1B], surfaceLevel);

		texCoord.y += 1;
		int index2A = edgeTable[int(texelFetch(triTable, texCoord, 0).x)][0];
		int index2B = edgeTable[int(texelFetch(triTable, texCoord, 0).x)][1];
		vec3 tempVertexPosB = lerpVector(vertexPositions[0][index2A], vertexPositions[0][index2B], noiseValues[0][index2A], noiseValues[0][index2B], surfaceLevel);

		texCoord.y += 1;
		int index3A = edgeTable[int(texelFetch(triTable, texCoord, 0).x)][0];
		int index3B = edgeTable[int(texelFetch(triTable, texCoord, 0).x)][1];
		vec3 tempVertexPosC = lerpVector(vertexPositions[0][index3A], vertexPositions[0][index3B], noiseValues[0][index3A], noiseValues[0][index3B], surfaceLevel);

		vec3 AB = tempVertexPosB - tempVertexPosA;
		vec3 AC = tempVertexPosC - tempVertexPosA;
		Normal.x = AB.y * AC.z - AB.z * AC.y;
		Normal.y = AB.z * AC.x - AB.x * AC.z;
		Normal.z = AB.x * AC.y - AB.y * AC.x;
		Normal = vec3(view * vec4(Normal, 0.0));

		gl_Position = projection * view * vec4(chunkPosition + cellPosition + tempVertexPosA, 1.0);
		FragPosView = vec3(view * vec4(chunkPosition + cellPosition + tempVertexPosA, 1.0));
		EmitVertex();

		gl_Position = projection * view * vec4(chunkPosition + cellPosition + tempVertexPosB, 1.0);
		FragPosView = vec3(view * vec4(chunkPosition + cellPosition + tempVertexPosB, 1.0));
		EmitVertex();

		gl_Position = projection * view * vec4(chunkPosition + cellPosition + tempVertexPosC, 1.0);
		FragPosView = vec3(view * vec4(chunkPosition + cellPosition + tempVertexPosC, 1.0));
		EmitVertex();

		EndPrimitive();
	}
}

vec3 lerpVector(vec3 a, vec3 b, float aValue, float bValue, float surfaceLevel) {
	return a + ((surfaceLevel - aValue) / (bValue - aValue)) * (b - a);
}
