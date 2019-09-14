#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 100) out;

out vec3 FragPosView;
out vec3 Normal;

in VS_OUT {
	vec3 chunkPosition;
} gs_in[];

uniform float cellLength;

// no model matrix necessary
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

void main() {
	vec3 chunkPosition = gs_in[0].chunkPosition;
	vec3 cellPosition = vec3(gl_in[0].gl_Position);

	// Front
	Normal = normal * vec3(0.0, 0.0, 1.0);

	vec3 vertexPosition = vec3(0.0, 0.0, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, 0.0, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, cellLength, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, cellLength, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	EndPrimitive();

	// Right
	Normal = normal * vec3(1.0, 0.0, 0.0);

	vertexPosition = vec3(cellLength, 0.0, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, 0.0, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, cellLength, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, cellLength, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	EndPrimitive();

	// Back
	Normal = normal * vec3(0.0, 0.0, -1.0);

	vertexPosition = vec3(cellLength, 0.0, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, 0.0, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, cellLength, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, cellLength, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	EndPrimitive();

	// Left
	Normal = normal * vec3(-1.0, 0.0, 0.0);

	vertexPosition = vec3(0.0, 0.0, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, 0.0, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, cellLength, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, cellLength, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	EndPrimitive();

	// Top
	Normal = normal * vec3(0.0, 1.0, 0.0);

	vertexPosition = vec3(0.0, cellLength, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, cellLength, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, cellLength, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, cellLength, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	EndPrimitive();

	// Bottom
	Normal = normal * vec3(0.0, 0.0, -1.0);

	vertexPosition = vec3(0.0, 0.0, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, 0.0, -cellLength);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(0.0, 0.0, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	vertexPosition = vec3(cellLength, 0.0, 0.0);
	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));
	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);
	EmitVertex();

	EndPrimitive();


	//FragPosView = vec3(view * model * vec4(aPos, 1.0));
	//Normal = normal * aNormal;
}
