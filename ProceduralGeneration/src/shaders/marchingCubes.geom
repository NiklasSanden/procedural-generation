#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 100) out;

out vec3 FragPosView;
out vec3 Normal;
out float NoiseValue;

in VS_OUT{
	vec3 chunkPosition;
	vec3 vertexPositions[8];
	double noiseValues[8];
} gs_in[];

uniform float cellLength;

// no model matrix necessary
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

void main() {
	const float cellLengthFactor = 0.5;
	vec3 chunkPosition = gs_in[0].chunkPosition;
	vec3 cellPosition = vec3(gl_in[0].gl_Position);

	gl_Position = projection * view * vec4(chunkPosition + cellPosition + gs_in[0].vertexPositions[0], 1.0);
	NoiseValue = float(gs_in[0].noiseValues[0]);
	EmitVertex();

	gl_Position = projection * view * vec4(chunkPosition + cellPosition + gs_in[0].vertexPositions[1], 1.0);
	NoiseValue = float(gs_in[0].noiseValues[1]);
	EmitVertex();

	gl_Position = projection * view * vec4(chunkPosition + cellPosition + gs_in[0].vertexPositions[2], 1.0);
	NoiseValue = float(gs_in[0].noiseValues[2]);
	EmitVertex();
	
	EndPrimitive();




	// ---------------------------
	// OLD
	// ---------------------------

	// Bottom
//	Normal = normal * vec3(0.0, 0.0, -1.0);
//
//	vertexPosition = vec3(0.0, 0.0, -cellLength * cellLengthFactor);
//	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
//	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
//	EmitVertex();

//	EndPrimitive();


	//FragPosView = vec3(view * model * vec4(aPos, 1.0));
	//Normal = normal * aNormal;
}

//	vec3 vertexPositions[] = {
//		vec3(0.0, 0.0, 0.0),
//		vec3(cellLength, 0.0, 0.0),
//		vec3(0.0, cellLength, 0.0),
//		vec3(cellLength, cellLength, 0.0),
//		vec3(0.0, 0.0, -cellLength),
//		vec3(cellLength, 0.0, -cellLength),
//		vec3(0.0, cellLength, -cellLength),
//		vec3(cellLength, cellLength, -cellLength)
//	};
