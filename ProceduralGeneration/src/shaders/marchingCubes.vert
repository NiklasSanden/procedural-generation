#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aChunkPosition;

out VS_OUT{
	vec3 chunkPosition;
} vs_out;

void main() {
    gl_Position = vec4(aPos, 1.0);
	vs_out.chunkPosition = aChunkPosition;
}
