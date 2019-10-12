#include "CPUMarchingCubesChunk.h"

#include "data/Noise.h"
#include "data/Tables.h"

#include "glad/glad.h"

#include "glm/glm.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include <thread>
#include <mutex>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

namespace ProceduralGeneration {
	// Specific order to make sure that the latter has at least one coordinate higher
	const int edgeTable[12][2] = { { 0, 1 }, { 1, 2 }, { 3, 2 }, { 0, 3 }, { 4, 5 }, { 5, 6 }, { 7, 6 }, { 4, 7 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 } };
}

std::mutex CPUMarchingCubesChunk::numberOfGeneratingChunksMutex;
int CPUMarchingCubesChunk::numberOfGeneratingChunks = 0;

CPUMarchingCubesChunk::CPUMarchingCubesChunk(const std::string& _name, const glm::vec3& _coordinates) : name(_name), coordinates(_coordinates) {
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

CPUMarchingCubesChunk::~CPUMarchingCubesChunk() {
	if (this->generator.joinable()) {
		this->generator.join();
	}
	
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBOPos);
	glDeleteBuffers(1, &this->VBONormals);
	glDeleteBuffers(1, &this->EBO);
}

void CPUMarchingCubesChunk::draw(int visibleChunks) {
	this->isGeneratedMutex.lock();
	if (this->isGenerated == false) {
		this->isGeneratedMutex.unlock();

		this->numberOfGeneratingChunksMutex.lock();
		if (numberOfGeneratingChunks > visibleChunks) {
			this->numberOfGeneratingChunksMutex.unlock();

			this->generator.join();
		}
		else {
			this->numberOfGeneratingChunksMutex.unlock();
		}

		return;
	}
	this->isGeneratedMutex.unlock();
	
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

void CPUMarchingCubesChunk::generateChunk(float chunkLength, int cellsPerAxis) {
	this->numberOfGeneratingChunksMutex.lock();
	numberOfGeneratingChunks++;
	this->numberOfGeneratingChunksMutex.unlock();

	this->generator = std::thread(&CPUMarchingCubesChunk::generateChunkThread, this, chunkLength, cellsPerAxis);
	//generateChunkThread(chunkLength, cellsPerAxis);
}

void CPUMarchingCubesChunk::generateChunkThread(float chunkLength, int cellsPerAxis) {
	glm::vec3 position = this->coordinates * chunkLength;
	float cellLength = chunkLength / cellsPerAxis;

	// Generate noise
	std::vector<std::vector<std::vector<float>>> noise(cellsPerAxis + 3ll, std::vector<std::vector<float>>(cellsPerAxis + 3ll, std::vector<float>(cellsPerAxis + 3ll, -1.0f)));
	generateNoise(cellsPerAxis, cellLength, position, noise);

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

	this->isGeneratedMutex.lock();
	this->isGenerated = true;
	this->isGeneratedMutex.unlock();

	this->numberOfGeneratingChunksMutex.lock();
	numberOfGeneratingChunks--;
	this->numberOfGeneratingChunksMutex.unlock();
}

void CPUMarchingCubesChunk::generateNoise(int cellsPerAxis, float cellLength, const glm::vec3& position, std::vector<std::vector<std::vector<float>>>& noise) {
	for (int x = 0; x < cellsPerAxis + 3; x++) {
		for (int y = 0; y < cellsPerAxis + 3; y++) {
			for (int z = 0; z < cellsPerAxis + 3; z++) {
				float xFloat = x - (cellsPerAxis + 2) / 2.0f; xFloat *= cellLength;
				float yFloat = y - (cellsPerAxis + 2) / 2.0f; yFloat *= cellLength;
				float zFloat = z - (cellsPerAxis + 2) / 2.0f; zFloat *= cellLength;

				float terraceHeight = 2.0f;
				
				noise[x][y][z] = -(yFloat + position.y) / 5.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 8, 0.5f, 0.2f, 1.0f) * 5.0f;
				//noise[x][y][z] = -(yFloat + position.y) / 5.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 4, 0.5f) * 2.0f + fmod((yFloat + position.y), 0.5f) / 5.0f;
				//noise[x][y][z] = 1.0f - glm::length(glm::vec3(xFloat + position.x, yFloat + position.y, zFloat + position.z)) / 20.0f + Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 4, 0.5f);
			}
		}
	}

	// Remove lonely points
	for (int xCount = 1; xCount < cellsPerAxis + 2; xCount++) {
		for (int yCount = 1; yCount < cellsPerAxis + 2; yCount++) {
			for (int zCount = 1; zCount < cellsPerAxis + 2; zCount++) {
				if (noise[xCount + 1ll][yCount][zCount] < surfaceLevel && noise[xCount - 1ll][yCount][zCount] < surfaceLevel &&
					noise[xCount][yCount + 1ll][zCount] < surfaceLevel && noise[xCount][yCount - 1ll][zCount] < surfaceLevel &&
					noise[xCount][yCount][zCount + 1ll] < surfaceLevel && noise[xCount][yCount][zCount - 1ll] < surfaceLevel) {
					
					noise[xCount][yCount][zCount] = 0.0f;
				}
			}
		}
	}
}

void CPUMarchingCubesChunk::calculateNormals(const std::vector<int>& indices, const std::vector<glm::vec3>& marginVertices) {
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

glm::vec3 CPUMarchingCubesChunk::lerpVector(glm::vec3 a, glm::vec3 b, float aValue, float bValue, float surfaceLevel) {
	//return (a + b) / 2.0f;
	return a + ((surfaceLevel - aValue) / (bValue - aValue)) * (b - a);
}
