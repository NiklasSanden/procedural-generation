#include "ComputeMCChunk.h"

#include "data/Noise.h"
#include "data/Tables.h"

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

namespace ProceduralGeneration {
	// Specific order to make sure that the latter has at least one coordinate higher
	const int edgeTable[12][2] = { { 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 }, { 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 } };
}

std::atomic<int> ComputeMCChunk::numberOfGeneratingChunks(0);

ComputeMCChunk::ComputeMCChunk(const std::string& _name, const glm::vec3& _coordinates, unsigned int _seed) : name(_name), coordinates(_coordinates), seed(_seed) {
	this->isGenerated.store(false);
	// -----------------------------
	// Setup vertex data
	// -----------------------------
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBOPos);
	glGenBuffers(1, &this->VBONormals);
	glGenBuffers(1, &this->EBO);

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
	if (this->generator.joinable()) {
		this->generator.join();
	}
	
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBOPos);
	glDeleteBuffers(1, &this->VBONormals);
	glDeleteBuffers(1, &this->EBO);
}

void ComputeMCChunk::draw(int visibleChunks) {
	if (this->isGenerated.load() == false) {
		if (numberOfGeneratingChunks.load() > visibleChunks) this->generator.join();
		return;
	}
	
	if (this->indexArray.size() == 0) {
		return;
	}
	else if (this->isSetup == false) {
		if (this->generator.joinable()) {
			this->generator.join();
		}

		// Set states
		glBindVertexArray(this->VAO);
		// Setup VBOPos
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOPos);
		glBufferData(GL_ARRAY_BUFFER, this->vertexPositions.size() * sizeof(float), this->vertexPositions.data(), GL_STATIC_DRAW);

		// Setup VBONormals
		glBindBuffer(GL_ARRAY_BUFFER, this->VBONormals);
		glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(float), this->normals.data(), GL_STATIC_DRAW);

		// Setup EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexArray.size() * sizeof(unsigned int), this->indexArray.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // this AFTER glBindVertexArray(0)

		this->isSetup = true;
	}

	// actually draw
	glBindVertexArray(this->VAO);

	glDrawElements(GL_TRIANGLES, (GLsizei)this->indexArray.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void ComputeMCChunk::generateChunk(float chunkLength, int cellsPerAxis) {
	numberOfGeneratingChunks++;

	this->generator = std::thread(&ComputeMCChunk::generateChunkThread, this, chunkLength, cellsPerAxis);
	//this->generator = std::thread(&ComputeMCChunk::generateChunkThreadCubes, this, chunkLength, cellsPerAxis);
	//generateChunkThread(chunkLength, cellsPerAxis);
}

void ComputeMCChunk::generateChunkThread(float chunkLength, int cellsPerAxis) {
	glm::vec3 position = this->coordinates * chunkLength;
	float cellLength = chunkLength / cellsPerAxis;

	// Generate noise
	std::vector<std::vector<std::vector<long double>>> noise(cellsPerAxis + 3ll, std::vector<std::vector<long double>>(cellsPerAxis + 3ll, std::vector<long double>(cellsPerAxis + 3ll, -1.0f)));
	generateNoise(cellsPerAxis + 3, cellLength, position, noise);

	// Generate vertices
	std::unordered_map<std::string, int> existingVertices;
	std::vector<int> temporaryIndices;		// Can contain negative integers to correlate to marginVertices
	std::vector<glm::vec3> marginVertices;

	for (int xCount = 0; xCount < cellsPerAxis + 2; xCount++) {
		for (int yCount = 0; yCount < cellsPerAxis + 2; yCount++) {
			for (int zCount = 0; zCount < cellsPerAxis + 2; zCount++) {
				std::vector<glm::ivec3> cornersArray = { glm::ivec3(xCount, yCount, zCount), glm::ivec3(xCount + 1, yCount, zCount), glm::ivec3(xCount + 1, yCount, zCount + 1), glm::ivec3(xCount, yCount, zCount + 1), glm::ivec3(xCount, yCount + 1, zCount), glm::ivec3(xCount + 1, yCount + 1, zCount), glm::ivec3(xCount + 1, yCount + 1, zCount + 1), glm::ivec3(xCount, yCount + 1, zCount + 1) };

				int cubeIndex = 0;
				for (int i = 0; i < cornersArray.size(); i++) {
					if (noise[cornersArray[i].x][cornersArray[i].y][cornersArray[i].z] > this->surfaceLevel) {
						cubeIndex |= 1 << i;
					}
				}

				unsigned char* triangulation = Tables::triangulationTable2D[cubeIndex];

				for (int i = 0; i < 16; i++) { 
					if (triangulation[i] == 32) break;

					glm::ivec3 cornerA = cornersArray[edgeTable[triangulation[i]][0]];
					glm::ivec3 cornerB = cornersArray[edgeTable[triangulation[i]][1]];

					int direction = -1; // 0 is direction x - 1 is direction y - 2 is direction z
					if (cornerB.x > cornerA.x) direction = 0;
					if (cornerB.y > cornerA.y) direction = 1;
					if (cornerB.z > cornerA.z) direction = 2;
					std::string vertexID = std::to_string(cornerA.x) + "," + std::to_string(cornerA.y) + "," + std::to_string(cornerA.z) + "," + std::to_string(direction);

					if (existingVertices.find(vertexID) != existingVertices.end()) {
						temporaryIndices.push_back(existingVertices[vertexID]);
					}
					else {
						glm::vec3 vertexPosition = lerpVector(cornerA, cornerB, noise[cornerA.x][cornerA.y][cornerA.z], noise[cornerB.x][cornerB.y][cornerB.z], this->surfaceLevel);
						vertexPosition *= cellLength;
						vertexPosition += position;
						vertexPosition -= glm::vec3((chunkLength + 2 * cellLength) / 2.0f, (chunkLength + 2 * cellLength) / 2.0f, (chunkLength + 2 * cellLength) / 2.0f);

						// If both corners are within the margin
						if (cornerA.x > 0 && cornerB.x > 0 && cornerA.y > 0 && cornerB.y > 0 && cornerA.z > 0 && cornerB.z > 0 &&
							cornerA.x < cellsPerAxis + 2 && cornerB.x < cellsPerAxis + 2 && cornerA.y < cellsPerAxis + 2 && cornerB.y < cellsPerAxis + 2 && cornerA.z < cellsPerAxis + 2 && cornerB.z < cellsPerAxis + 2) {
							existingVertices[vertexID] = (int)(this->vertexPositions.size() / 3);
							temporaryIndices.push_back((int)(this->vertexPositions.size() / 3));

							this->vertexPositions.push_back(vertexPosition.x);
							this->vertexPositions.push_back(vertexPosition.y);
							this->vertexPositions.push_back(vertexPosition.z);
						}
						else {
							existingVertices[vertexID] = -(int)marginVertices.size() - 1;
							temporaryIndices.push_back(-(int)marginVertices.size() - 1);

							marginVertices.push_back(vertexPosition);
						}
					}
				}
			}
		}
	}
	// Calculate normals
	calculateNormals(temporaryIndices, marginVertices);

	// Correct indices
	for (int i = 0; i < temporaryIndices.size(); i += 3) {
		int indexA = temporaryIndices[i];
		int indexB = temporaryIndices[i + 1ll];
		int indexC = temporaryIndices[i + 2ll];

		if (indexA < 0 || indexB < 0 || indexC < 0) continue;

		this->indexArray.push_back((unsigned int)indexA);
		this->indexArray.push_back((unsigned int)indexB);
		this->indexArray.push_back((unsigned int)indexC);
	}

	this->isGenerated.store(true);

	numberOfGeneratingChunks--;
}

void ComputeMCChunk::generateChunkThreadCubes(float chunkLength, int cellsPerAxis) {
	glm::vec3 position = this->coordinates * chunkLength;
	float cellLength = chunkLength / cellsPerAxis;

	// Generate noise
	std::vector<std::vector<std::vector<long double>>> noise(cellsPerAxis + 2ll, std::vector<std::vector<long double>>(cellsPerAxis + 2ll, std::vector<long double>(cellsPerAxis + 2ll, -1.0f)));
	generateNoise(cellsPerAxis + 2ll, cellLength, position, noise);
	
	// Generate vertices
	std::vector<int> intIndices;

	for (int xCount = 1; xCount < cellsPerAxis + 1; xCount++) {
		for (int yCount = 1; yCount < cellsPerAxis + 1; yCount++) {
			for (int zCount = 1; zCount < cellsPerAxis + 1; zCount++) {
				if (noise[xCount][yCount][zCount] > this->surfaceLevel) {
					continue;
				}

				std::vector<glm::ivec3> adjCells = { glm::ivec3(xCount - 1, yCount, zCount), glm::ivec3(xCount + 1, yCount, zCount), glm::ivec3(xCount, yCount - 1, zCount), glm::ivec3(xCount, yCount + 1, zCount), glm::ivec3(xCount, yCount, zCount - 1), glm::ivec3(xCount, yCount, zCount + 1) };
				std::vector<glm::ivec3> cornersArray = { glm::ivec3(xCount, yCount, zCount), glm::ivec3(xCount + 1, yCount, zCount), glm::ivec3(xCount + 1, yCount, zCount + 1), glm::ivec3(xCount, yCount, zCount + 1), glm::ivec3(xCount, yCount + 1, zCount), glm::ivec3(xCount + 1, yCount + 1, zCount), glm::ivec3(xCount + 1, yCount + 1, zCount + 1), glm::ivec3(xCount, yCount + 1, zCount + 1) };

				bool adjNoiseResults[6] = { false, false, false, false, false, false };
				for (int i = 0; i < adjCells.size(); i++) {
					if (noise[adjCells[i].x][adjCells[i].y][adjCells[i].z] > this->surfaceLevel) {
						adjNoiseResults[i] = true;
					}
				}

				std::vector<unsigned char> triangulation;
				// Top bottom
				if (adjNoiseResults[2]) {
					triangulation.push_back(0);
					triangulation.push_back(2);
					triangulation.push_back(1);

					triangulation.push_back(3);
					triangulation.push_back(2);
					triangulation.push_back(0);
				}
				if (adjNoiseResults[3]) {
					triangulation.push_back(4);
					triangulation.push_back(5);
					triangulation.push_back(6);

					triangulation.push_back(6);
					triangulation.push_back(7);
					triangulation.push_back(4);
				}

				// Right left
				if (adjNoiseResults[0]) {
					triangulation.push_back(0);
					triangulation.push_back(4);
					triangulation.push_back(7);

					triangulation.push_back(7);
					triangulation.push_back(3);
					triangulation.push_back(0);
				}
				if (adjNoiseResults[1]) {
					triangulation.push_back(1);
					triangulation.push_back(2);
					triangulation.push_back(5);

					triangulation.push_back(5);
					triangulation.push_back(2);
					triangulation.push_back(6);
				}

				// Front back
				if (adjNoiseResults[4]) {
					triangulation.push_back(0);
					triangulation.push_back(1);
					triangulation.push_back(4);

					triangulation.push_back(4);
					triangulation.push_back(1);
					triangulation.push_back(5);
				}
				if (adjNoiseResults[5]) {
					triangulation.push_back(2);
					triangulation.push_back(3);
					triangulation.push_back(7);

					triangulation.push_back(2);
					triangulation.push_back(7);
					triangulation.push_back(6);
				}

				std::unordered_map<std::string, int> existingVertices;
				for (int i = 0; i < triangulation.size(); i++) {
					if (i % 6 == 0) existingVertices.clear();

					glm::ivec3 corner = cornersArray[triangulation[i]];

					std::string vertexID = std::to_string(corner.x) + "," + std::to_string(corner.y) + "," + std::to_string(corner.z);

					if (existingVertices.find(vertexID) != existingVertices.end()) {
						this->indexArray.push_back((unsigned int)existingVertices[vertexID]);
						intIndices.push_back(this->indexArray.back());
					}
					else {
						glm::vec3 vertexPosition = corner;
						vertexPosition *= cellLength;
						vertexPosition += position;
						vertexPosition -= glm::vec3(chunkLength / 2.0f, chunkLength / 2.0f, chunkLength / 2.0f);

						// If both corners are within the margin
						this->indexArray.push_back((unsigned int)(this->vertexPositions.size() / 3));
						intIndices.push_back(this->indexArray.back());

						existingVertices[vertexID] = (int)(this->vertexPositions.size() / 3);

						this->vertexPositions.push_back(vertexPosition.x);
						this->vertexPositions.push_back(vertexPosition.y);
						this->vertexPositions.push_back(vertexPosition.z);
					}
				}
			}
		}
	}
	// Calculate normals
	calculateNormals(intIndices, {});

	this->isGenerated.store(true);

	numberOfGeneratingChunks--;
}

void ComputeMCChunk::generateNoise(int pointsPerAxis, float pointsSpace, const glm::vec3& position, std::vector<std::vector<std::vector<long double>>>& noise) {
	for (int x = 0; x < pointsPerAxis; x++) {
		for (int y = 0; y < pointsPerAxis; y++) {
			for (int z = 0; z < pointsPerAxis; z++) {
				long double xFloat = x - (pointsPerAxis - 1ll) / 2.0; xFloat *= pointsSpace;
				long double yFloat = y - (pointsPerAxis - 1ll) / 2.0; yFloat *= pointsSpace;
				long double zFloat = z - (pointsPerAxis - 1ll) / 2.0; zFloat *= pointsSpace;
				
				// Regular noise
				//noise[x][y][z] = Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 8, 0.5f, 0.2f, this->seed);

				// Ground
				//noise[x][y][z] = -(yFloat + position.y) / 5.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 8, 0.5f, 0.2f, this->seed) * 5.0f;
				
				// Warp
				/*long double warp = Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 1, 0.4f, 0.04f, this->seed);
				xFloat += warp * 10;
				zFloat += warp * 10;
				noise[x][y][z] = -(yFloat + position.y) / 10.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y + warp * 10, zFloat + position.z, 9, 0.4f, 0.1f, this->seed);*/

				// Terrace ground
				//noise[x][y][z] = -(yFloat + position.y) / 5.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 4, 0.5f, 0.2f, this->seed) * 2.0f + fmod((yFloat + position.y), 0.5f) / 5.0f;
				
				// Minecraft
				float tempNoise = Noise::octavePerlin(xFloat + position.x, 0.0f, zFloat + position.z, 1, 0.3f, 0.015f, this->seed);
				float tempA = -(position.y + yFloat) / 10.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 4, 0.3f, 0.05f, this->seed);
				float tempB = (-(position.y + yFloat) + 40.0f) / 30.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 4, 0.3f, 0.05f, this->seed) * 1.5f;

				tempA = glm::clamp(tempA, -1.0f, 1.0f);
				float groundNoise = lerpFloat(tempB, tempA, glm::min(1.0f, tempNoise * tempNoise + tempNoise));
				
				float undergroundNoise = 0.075f + 2 * this->surfaceLevel - Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 4, 0.3f, 0.06f, this->seed);
				
				float groundAndUndergroundLerp = glm::clamp(((yFloat + position.y) - ((long double)20.0f * (1.0f - tempNoise))) / 20.0f, (long double)0.0, (long double)1.0);
				noise[x][y][z] = lerpFloat(undergroundNoise, groundNoise, groundAndUndergroundLerp);
				
				// Circle
				//noise[x][y][z] = 1.0f - glm::length(glm::vec3(xFloat + position.x, yFloat + position.y, zFloat + position.z)) / 20.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 4, 0.5f, 0.2f, this->seed);
			}
		}
	}

	// Remove lonely points
	for (int xCount = 1; xCount < noise.size() - 1; xCount++) {
		for (int yCount = 1; yCount < noise.size() - 1; yCount++) {
			for (int zCount = 1; zCount < noise.size() - 1; zCount++) {
				if (noise[xCount + 1ll][yCount][zCount] < surfaceLevel && noise[xCount - 1ll][yCount][zCount] < surfaceLevel &&
					noise[xCount][yCount + 1ll][zCount] < surfaceLevel && noise[xCount][yCount - 1ll][zCount] < surfaceLevel &&
					noise[xCount][yCount][zCount + 1ll] < surfaceLevel && noise[xCount][yCount][zCount - 1ll] < surfaceLevel) {
					
					//noise[xCount][yCount][zCount] = 0.0f;
				}
			}
		}
	}
}

void ComputeMCChunk::calculateNormals(const std::vector<int>& indices, const std::vector<glm::vec3>& marginVertices) {
	std::vector<glm::vec3> normalArray(this->vertexPositions.size() / 3, glm::vec3(0.0f));

	for (int i = 0; i < indices.size(); i += 3) {
		glm::vec3 vertexA = indices[i + 0ll] < 0 ? marginVertices[glm::abs(indices[i + 0ll]) - 1ll] : glm::vec3(this->vertexPositions[indices[i + 0ll] * 3ll + 0ll], this->vertexPositions[indices[i + 0ll] * 3ll + 1ll], this->vertexPositions[indices[i + 0ll] * 3ll + 2ll]);
		glm::vec3 vertexB = indices[i + 1ll] < 0 ? marginVertices[glm::abs(indices[i + 1ll]) - 1ll] : glm::vec3(this->vertexPositions[indices[i + 1ll] * 3ll + 0ll], this->vertexPositions[indices[i + 1ll] * 3ll + 1ll], this->vertexPositions[indices[i + 1ll] * 3ll + 2ll]);
		glm::vec3 vertexC = indices[i + 2ll] < 0 ? marginVertices[glm::abs(indices[i + 2ll]) - 1ll] : glm::vec3(this->vertexPositions[indices[i + 2ll] * 3ll + 0ll], this->vertexPositions[indices[i + 2ll] * 3ll + 1ll], this->vertexPositions[indices[i + 2ll] * 3ll + 2ll]);
	
		if (vertexA == vertexB || vertexB == vertexC || vertexA == vertexC) {
			if (indices[i + 0ll] >= 0) normalArray[indices[i + 0ll]] += glm::vec3(0.0f, 0.0f, 0.0f);
			if (indices[i + 1ll] >= 0) normalArray[indices[i + 1ll]] += glm::vec3(0.0f, 0.0f, 0.0f);
			if (indices[i + 2ll] >= 0) normalArray[indices[i + 2ll]] += glm::vec3(0.0f, 0.0f, 0.0f);
			continue;
		}

		glm::vec3 AB = vertexB - vertexA;
		glm::vec3 AC = vertexC - vertexA;

		glm::vec3 normal = glm::normalize(glm::cross(AB, AC));

		float area = glm::abs(glm::sqrt(glm::pow(AB.y * AC.z - AB.z * AC.y, 2) +
										glm::pow(AB.z * AC.x - AB.x * AC.z, 2) +
										glm::pow(AB.x * AC.y - AB.y * AC.x, 2))) / 2.0f;

		if (area == 0.0f) {
			Engine::LogManager::LogError("Area is zero");
		}
		if (indices[i + 0ll] >= 0) normalArray[indices[i + 0ll]] += normal / area;
		if (indices[i + 1ll] >= 0) normalArray[indices[i + 1ll]] += normal / area;
		if (indices[i + 2ll] >= 0) normalArray[indices[i + 2ll]] += normal / area;
	}

	for (int i = 0; i < normalArray.size(); i++) {
		normalArray[i] = glm::normalize(normalArray[i]);

		this->normals.push_back(normalArray[i].x);
		this->normals.push_back(normalArray[i].y);
		this->normals.push_back(normalArray[i].z);
	}
}

glm::vec3 ComputeMCChunk::lerpVector(glm::vec3 a, glm::vec3 b, float aValue, float bValue, float surfaceLevel) {
	//return aValue > bValue ? a : b;
	//return (a + b) / 2.0f;
	return a + ((surfaceLevel - aValue) / (bValue - aValue)) * (b - a);
}

float ComputeMCChunk::lerpFloat(float aValue, float bValue, float surfaceLevel) {
	float returnValue = aValue + surfaceLevel * (bValue - aValue);
	return returnValue;
}
