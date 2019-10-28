#include "ComputeMCChunk.h"

#include "data/Noise.h"
#include "data/Tables.h"
#include "engine/resources/ResourceManager.h"
#include "engine/resources/Shader.h"

#include "glad/glad.h"

#include "glm/glm.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include <thread>
#include <atomic>
#include <iostream>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

ComputeMCChunk::ComputeMCChunk(const std::string& _name, const glm::vec3& _coordinates, unsigned int _seed) : name(_name), coordinates(_coordinates), seed(_seed) {
	this->noiseComputeShader = Engine::ResourceManager::createShaderProgram({ "noise.comp" }, "NoiseComputeShader");
	
	this->mcComputeShader = Engine::ResourceManager::createShaderProgram({ "mc.comp" }, "MCComputeShader");
	this->mcComputeShader->use();
	glUniform1i(glGetUniformLocation(this->mcComputeShader->ID, "triTable"), 1);


	glGenTextures(1, &this->noiseTextureID);
	
	
	// -----------------------------
	// Setup vertex data
	// -----------------------------
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBOPos);
	glGenBuffers(1, &this->VBONormals);
	glGenBuffers(1, &this->tempVBOPos);
	glGenBuffers(1, &this->tempVBONormals);
	glGenBuffers(1, &this->atomicCounter);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBOPos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBONormals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ComputeMCChunk::~ComputeMCChunk() {
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBOPos);
	glDeleteBuffers(1, &this->VBONormals);

	//glDeleteBuffers(1, &this->tempVBOPos);
	//glDeleteBuffers(1, &this->tempVBONormals);
	//glDeleteBuffers(1, &this->atomicCounter);

	//glDeleteTextures(1, &this->noiseTextureID);
}

bool ComputeMCChunk::draw(unsigned int shaderID) {
	if (this->hasSetup == false) {
		// Make sure the marching cubes is finished
		//glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		// glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, this->atomicCounter);
		glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned int), &this->amountOfVertices);
		this->amountOfVertices *= 3;

		// No longer necessary
		glDeleteTextures(1, &this->noiseTextureID);
		glDeleteBuffers(1, &this->atomicCounter);

		if (this->amountOfVertices == 0) {
			glDeleteBuffers(1, &this->tempVBOPos);
			glDeleteBuffers(1, &this->tempVBONormals);

			return true;
		}

		// Copy data to an appropriately sized buffer
		GLint bufferSize = this->amountOfVertices * 3 * (int)sizeof(float);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBOPos);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
		glCopyNamedBufferSubData(this->tempVBOPos, this->VBOPos, 0, 0, bufferSize);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBONormals);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
		glCopyNamedBufferSubData(this->tempVBONormals, this->VBONormals, 0, 0, bufferSize);


		glDeleteBuffers(1, &this->tempVBOPos);
		glDeleteBuffers(1, &this->tempVBONormals);
		this->hasSetup = true;
	}

	// Set LOD
	glUniform1i(glGetUniformLocation(shaderID, "LOD"), this->name.back() - '0');

	glBindVertexArray(this->VAO);

	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(this->amountOfVertices));

	glBindVertexArray(0);
	return false;
}

void ComputeMCChunk::generateChunk(float chunkLength, int cellsPerAxis) {
	glm::vec3 position = this->coordinates * chunkLength;
	float cellLength = chunkLength / cellsPerAxis;

	// Generate noise
	generateNoise(cellsPerAxis + 5, cellLength, position);

	// Generate mesh
	generateMesh(cellsPerAxis, cellLength, position);
}

void ComputeMCChunk::generateNoise(int pointsPerAxis, float cellLength, const glm::vec3& position) {
	glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
	srand(this->seed);
	offset.x += glm::linearRand(-10000.0f, 10000.0f);
	offset.y += glm::linearRand(-10000.0f, 10000.0f);
	offset.z += glm::linearRand(-10000.0f, 10000.0f);
	
	this->noiseComputeShader->use();

	// Uniforms
	this->noiseComputeShader->setVec3("position", position);
	this->noiseComputeShader->setVec3("offset", offset);
	this->noiseComputeShader->setFloat("cellLength", cellLength);
	this->noiseComputeShader->setInt("pointsPerAxis", pointsPerAxis);
	this->noiseComputeShader->setFloat("surfaceLevel", this->surfaceLevel);
	this->noiseComputeShader->setInt("LOD", this->name.back() - '0');

	//Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->noiseTextureID);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, pointsPerAxis, pointsPerAxis, pointsPerAxis, 0, GL_RED, GL_FLOAT, NULL);
	glBindImageTexture(0, this->noiseTextureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

	// Compute shader
	glDispatchCompute((GLuint)pointsPerAxis, (GLuint)pointsPerAxis, (GLuint)pointsPerAxis);
}

void ComputeMCChunk::generateMesh(int cellsPerAxis, float cellLength, const glm::vec3& position) {
	this->mcComputeShader->use();
	Tables::activateTriangulationTable(GL_TEXTURE1);

	// Uniforms
	this->mcComputeShader->setFloat("surfaceLevel", 0.0f);
	this->mcComputeShader->setFloat("cellLength", cellLength);
	this->mcComputeShader->setInt("cellsPerAxis", cellsPerAxis);
	this->mcComputeShader->setVec3("chunkPosition", position);

	// SSBOs
	GLint bufferSize = cellsPerAxis * cellsPerAxis * cellsPerAxis * 15 * 3 * (int)sizeof(float) / 2; // / 2 is to make the buffers smaller, and it is unlikely that you would need more vertices anyway
	glBindBuffer(GL_ARRAY_BUFFER, this->tempVBOPos);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->tempVBOPos);

	glBindBuffer(GL_ARRAY_BUFFER, this->tempVBONormals);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, this->tempVBONormals);

	unsigned int atomicCounterValue = 0u;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, this->atomicCounter);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &atomicCounterValue, GL_DYNAMIC_READ);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, this->atomicCounter);


	// Make sure the noise has finished generating
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Set the noise texture
	glUniform1i(glGetUniformLocation(this->mcComputeShader->ID, "noise"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->noiseTextureID);

	// Compute shader
	glDispatchCompute((GLuint)cellsPerAxis, (GLuint)cellsPerAxis, (GLuint)cellsPerAxis);
}
