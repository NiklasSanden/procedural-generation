#version 450 core
layout (location = 0) in vec3 aCellPos;

uniform sampler3D noise;
uniform vec3 chunkPosition;
uniform float cellLength;
uniform float cellsPerAxis;

out VS_OUT{
	vec3 chunkPosition;
	vec3 vertexPositions[8];
	float noiseValues[8];
} vs_out;

void main() {
    //gl_Position = vec4(aCellPos, 1.0);
    gl_Position = vec4(vec3(0, 0, 0), 1.0);
	vs_out.chunkPosition = chunkPosition;
	
	vs_out.vertexPositions[0] = vec3(0.0, 0.0, 0.0);
	vs_out.vertexPositions[1] = vec3(0.0, 0.0, -cellLength);
	vs_out.vertexPositions[2] = vec3(cellLength, 0.0, -cellLength);
	vs_out.vertexPositions[3] = vec3(cellLength, 0.0, 0.0);
	vs_out.vertexPositions[4] = vec3(0.0, cellLength, 0.0);
	vs_out.vertexPositions[5] = vec3(0.0, cellLength, -cellLength);
	vs_out.vertexPositions[6] = vec3(cellLength, cellLength, -cellLength);
	vs_out.vertexPositions[7] = vec3(cellLength, cellLength, 0.0);

	for (int i = 0; i < 8; i++) {
		vs_out.noiseValues[i] = 1;
		//vs_out.noiseValues[i] = texture(noise,
		//								vec3((aCellPos.x + vs_out.vertexPositions[i].x) / cellLength / cellsPerAxis,
		//									 (aCellPos.y + vs_out.vertexPositions[i].y) / cellLength / cellsPerAxis,
		//									 (aCellPos.z + vs_out.vertexPositions[i].z) / cellLength / cellsPerAxis)).x;
	}
}
