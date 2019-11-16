#include "objects/Complete/CompleteMCManager.h"

#include "engine/misc/Game.h"
#include "engine/resources/ResourceManager.h"
#include "engine/resources/Texture2D.h"
#include "engine/resources/Material.h"
#include "engine/components/Transform.h"
#include "engine/resources/Shader.h"
#include "engine/objects/Lights.h"
#include "engine/components/MeshRenderer.h"
#include "engine/misc/Program.h"
#include "setup/GameManager.h"
#include "setup/Camera.h"
#include "data/Tables.h"
#include "data/NoiseTexture.h"

#include "objects/Complete/VertexBuffer.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <queue>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

CompleteMCManager::CompleteMCManager(const std::string& name) : GameObject(name) {
	this->water = new Water(this->cellsPerAxis + 1);
	this->cubemap = new Cubemap();
	// Material
	this->material = new Engine::Material(glm::vec3(0.7f, 0.2f, 0.1f), glm::vec3(0.125f, 0.125f, 0.125f), 4);

	// Shader program
	this->renderingShaders = Engine::ResourceManager::createShaderProgram({ "completeMC.vert", "completeMC.frag" }, "CompleteMCRenderShader");

	this->noiseShaders = Engine::ResourceManager::createShaderProgram({ "completeNoise.comp" }, "CompleteNoiseComputeShader");
	this->mcShaders = Engine::ResourceManager::createShaderProgram({ "completeMC.comp" }, "CompleteMCComputeShader");

	// Generate buffers 
	glGenBuffers(1, &this->writableVBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->writableVBO);
	glBufferData(GL_ARRAY_BUFFER, (long long)this->cellsPerAxis * this->cellsPerAxis * this->cellsPerAxis * 15 * 6 * sizeof(float), NULL, GL_DYNAMIC_COPY);

	glGenBuffers(1, &this->atomicCounter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, this->atomicCounter);
	unsigned int atomicCounterValue = 0u;
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &atomicCounterValue, GL_DYNAMIC_READ);

	// Generate texture
	glGenTextures(1, &this->noiseTextureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->noiseTextureID);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, this->cellsPerAxis + 5, this->cellsPerAxis + 5, this->cellsPerAxis + 5, 0, GL_RED, GL_FLOAT, NULL);
	//glBindImageTexture(0, this->noiseTextureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);


	glGenTextures(1, &this->preCalculatedNoiseTextureID);

	glBindTexture(GL_TEXTURE_3D, this->preCalculatedNoiseTextureID);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	std::vector<float> preCalculatedNoiseData(16 * 16 * 16 * 4, 0.0f);
	getPrecalculatedNoiseDate(preCalculatedNoiseData);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, 16, 16, 16, 0, GL_RGBA, GL_FLOAT, preCalculatedNoiseData.data());

	// VBO Manager
	this->vboManager = new VBOManager(this->numberOfVertexBuffers);
}

CompleteMCManager::~CompleteMCManager() {
	delete this->water;
	delete this->cubemap;

	delete this->material;

	delete this->vboManager;

	for (auto pair : this->generatedChunks) {
		delete pair.second;
	}

	glDeleteTextures(1, &this->noiseTextureID);
	glDeleteTextures(1, &this->preCalculatedNoiseTextureID);

	glDeleteBuffers(1, &this->writableVBO);
	glDeleteBuffers(1, &this->atomicCounter);
}

void CompleteMCManager::regenerateChunks() {
	this->water->regenerate(this->cellsPerAxis + 1);

	// Clear empty chunks
	this->emptyChunks.clear();

	// If cellsPerAxis changes:
	glBindBuffer(GL_ARRAY_BUFFER, this->writableVBO);
	glBufferData(GL_ARRAY_BUFFER, this->cellsPerAxis * this->cellsPerAxis * this->cellsPerAxis * 15 * 6 * (int)sizeof(float), NULL, GL_DYNAMIC_COPY);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->noiseTextureID);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, this->cellsPerAxis + 5, this->cellsPerAxis + 5, this->cellsPerAxis + 5, 0, GL_RED, GL_FLOAT, NULL);

	// Clear generated chunks
	for (auto pair : this->generatedChunks) {
		this->vboManager->storeUnoccupiedVBO(pair.second);
	}
	this->generatedChunks.clear();

	// Reset VBOManager
	delete this->vboManager;
	this->vboManager = new VBOManager(this->numberOfVertexBuffers);

	// Recalculate noise
	glBindTexture(GL_TEXTURE_3D, this->preCalculatedNoiseTextureID);

	std::vector<float> preCalculatedNoiseData(16 * 16 * 16 * 4, 0.0f);
	getPrecalculatedNoiseDate(preCalculatedNoiseData);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, 16, 16, 16, 0, GL_RGBA, GL_FLOAT, preCalculatedNoiseData.data());
}

void CompleteMCManager::getPrecalculatedNoiseDate(std::vector<float>& preCalculatedNoiseData) {
	srand(this->seed);
	/*for (int i = 0; i < 16 * 16 * 16; i++) {
		preCalculatedNoiseData[i * 4    ] = glm::linearRand(0.0f, 1.0f);
		preCalculatedNoiseData[i * 4 + 1] = glm::linearRand(0.0f, 1.0f);
		preCalculatedNoiseData[i * 4 + 2] = glm::linearRand(0.0f, 1.0f);
		preCalculatedNoiseData[i * 4 + 3] = glm::linearRand(0.0f, 1.0f);
	}*/
	int index = 0;
	glm::vec3 offset1(glm::linearRand(-100.0f, 100.0f), glm::linearRand(-100.0f, 100.0f), glm::linearRand(-100.0f, 100.0f));
	glm::vec3 offset2(glm::linearRand(-100.0f, 100.0f), glm::linearRand(-100.0f, 100.0f), glm::linearRand(-100.0f, 100.0f));
	glm::vec3 offset3(glm::linearRand(-100.0f, 100.0f), glm::linearRand(-100.0f, 100.0f), glm::linearRand(-100.0f, 100.0f));
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				preCalculatedNoiseData[index * 4    ] = (Noise::octavePerlin(x			  , y			 , z			, 2, 0.4, 0.51, this->seed, 1.0) + 1.0f) / 2.0f;
				preCalculatedNoiseData[index * 4 + 1] = (Noise::octavePerlin(x + offset1.x, y + offset1.y, z + offset1.z, 2, 0.4, 0.51, this->seed, 1.0) + 1.0f) / 2.0f;
				preCalculatedNoiseData[index * 4 + 2] = (Noise::octavePerlin(x + offset2.x, y + offset2.y, z + offset2.z, 2, 0.4, 0.51, this->seed, 1.0) + 1.0f) / 2.0f;
				preCalculatedNoiseData[index * 4 + 3] = (Noise::octavePerlin(x + offset3.x, y + offset3.y, z + offset3.z, 2, 0.4, 0.51, this->seed, 1.0) + 1.0f) / 2.0f;
				index++;
			}
		}
	}
}

void CompleteMCManager::update(float deltaTime) {
	//double tempTime = glfwGetTime();
	Camera* player = GameManager::getPlayer();

	//this->emptyChunks.clear();

	// Used by all chunks to determine if it can be seen or not
	// calculate horizontal fov from vertical: https://www.gamedev.net/forums/topic/361241-horizontal-fov/
	float halfVerticalFov = glm::radians(Camera::verticalFov / 2.0f);
	float aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
	float halfHorizontalFov = 2.0f * glm::atan(glm::tan(glm::radians(Camera::verticalFov) * 0.5f) * aspectRatio) / 2.0f;
	glm::vec3 rightProjectionNormal = glm::normalize(glm::cross(player->transform->getUp(), glm::angleAxis(-halfHorizontalFov, player->transform->getUp()) * -player->transform->getDirection()));
	glm::vec3 leftProjectionNormal = glm::normalize(glm::cross(glm::angleAxis(halfHorizontalFov, player->transform->getUp()) * -player->transform->getDirection(), player->transform->getUp()));

	glm::vec3 upProjectionNormal = glm::normalize(glm::cross(glm::angleAxis(halfVerticalFov, player->transform->getRight()) * -player->transform->getDirection(), player->transform->getRight()));
	glm::vec3 downProjectionNormal = glm::normalize(glm::cross(player->transform->getRight(), glm::angleAxis(-halfVerticalFov, player->transform->getRight()) * -player->transform->getDirection()));

	float farthestViewDistanceFactor = (1.0f / glm::cos(halfHorizontalFov)) / glm::cos(halfVerticalFov);

	std::vector<std::string> chunksToBeRendered;
	std::vector<glm::ivec3> nonEmptyChunksPos;

	//updateActiveChunks(chunksToBeRendered, nonEmptyChunksPos, this->chunkLength * 4, 4, farthestViewDistanceFactor, this->numberOfVertexBuffers, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);
	updateActiveChunks(chunksToBeRendered, nonEmptyChunksPos, this->chunkLength    , 1, farthestViewDistanceFactor, this->numberOfVertexBuffers, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);
	
	int amountForLOD2 = (this->numberOfVertexBuffers - chunksToBeRendered.size()) / 2;
	int amountForLOD1 = this->numberOfVertexBuffers - chunksToBeRendered.size() - amountForLOD2;

	int sizeBeforeLOD2 = (int)nonEmptyChunksPos.size();

	//updateActiveChunks(chunksToBeRendered, nonEmptyChunksPos, this->chunkLength * 2, 2, farthestViewDistanceFactor, amountForLOD2, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);
	//updateActiveChunks(chunksToBeRendered, nonEmptyChunksPos, this->chunkLength    , 1, farthestViewDistanceFactor, amountForLOD1, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);
	//updateDetailedChunks(chunksToBeRendered, nonEmptyChunksPos, this->chunkLength * 2, 0, 2, farthestViewDistanceFactor, amountForLOD2, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);
	//updateDetailedChunks(chunksToBeRendered, nonEmptyChunksPos, this->chunkLength    , sizeBeforeLOD2, 1, farthestViewDistanceFactor, amountForLOD1, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);


	std::unordered_map<std::string, VertexBuffer*> oldChunks = this->generatedChunks;
	this->generatedChunks.clear();

	std::vector<std::string> chunksToGenerate;

	for (unsigned int i = 0; i < (unsigned int)chunksToBeRendered.size(); i++) {
		if (i >= this->numberOfVertexBuffers) break;

		if (oldChunks.find(chunksToBeRendered[i]) == oldChunks.end()) {
			chunksToGenerate.push_back(chunksToBeRendered[i]);
		}
		else {
			this->generatedChunks[chunksToBeRendered[i]] = oldChunks[chunksToBeRendered[i]];
			oldChunks.erase(chunksToBeRendered[i]);
		}
	}

	for (auto chunk : oldChunks) {
		this->vboManager->storeUnoccupiedVBO(chunk.second);
	}

	if (chunksToGenerate.size() != 0ll) {
		// Textures
		Tables::activateTriangulationTable(GL_TEXTURE4);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, this->noiseTextureID);
		glBindImageTexture(0, this->noiseTextureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, this->preCalculatedNoiseTextureID);

		// Buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->writableVBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->writableVBO);

		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, this->atomicCounter);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, this->atomicCounter);

		// Uniforms
		glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
		srand(this->seed);
		offset.x += glm::linearRand(-1000.0f, 1000.0f);
		offset.y += glm::linearRand(-1000.0f, 1000.0f);
		offset.z += glm::linearRand(-1000.0f, 1000.0f);

		this->noiseShaders->use();
		this->noiseShaders->setVec3("offset", offset);
		this->noiseShaders->setInt("pointsPerAxis", this->cellsPerAxis + 5);

		this->mcShaders->use();
		this->mcShaders->setFloat("surfaceLevel", 0.0f);
		this->mcShaders->setInt("cellsPerAxis", this->cellsPerAxis);

		int generatedThisFrame = 0;
		for (int i = 0; i < (int)chunksToGenerate.size() && generatedThisFrame < 300; i++) {
			VertexBuffer* vboPtr = this->vboManager->getUnoccupiedVBO();
			
			bool isEmpty = generateChunk(vboPtr, chunksToGenerate[i]);

			if (isEmpty) {
				this->emptyChunks.insert(chunksToGenerate[i]);
				this->vboManager->storeUnoccupiedVBO(vboPtr);
			}
			else {
				this->generatedChunks[chunksToGenerate[i]] = vboPtr;
				generatedThisFrame++;
			}
		}
	}

	//std::cout << (glfwGetTime() - tempTime) * 1000.0f << std::endl;
}

void CompleteMCManager::updateActiveChunks(std::vector<std::string>& chunksToBeRendered, std::vector<glm::ivec3>& nonEmptyChunksPos, float chunkLength, int LODIndex, float farthestViewDistanceFactor, int maxChunksForLOD, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal) {

	Camera* player = GameManager::getPlayer();
	glm::vec3 playerPosition = player->transform->getPosition();
	glm::ivec3 roundedPlayerPosition = glm::round(playerPosition / chunkLength);


	float chunkDistanceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
	// Since the view distance is how far away a plane is, we need the distance to the farthest point on that plane
	float farthestViewDistance = Camera::viewDistance * farthestViewDistanceFactor;

	std::queue<glm::ivec3> chunks;
	chunks.push(roundedPlayerPosition);

	std::unordered_set<std::string> visitedChunks;

	int amountsOfChunks = 0;
	while (chunks.size() > 0ll && amountsOfChunks < maxChunksForLOD) {
		glm::ivec3 currentChunkCoords = chunks.front();
		chunks.pop();

		std::string currentChunkName = std::to_string(currentChunkCoords.x) + " " + std::to_string(currentChunkCoords.y) + " " + std::to_string(currentChunkCoords.z) + " " + std::to_string(LODIndex);
		if (visitedChunks.find(currentChunkName) != visitedChunks.end()) continue;
		visitedChunks.insert(currentChunkName);
		

		glm::vec3 currentChunkWorldPos = glm::vec3(currentChunkCoords) * chunkLength;

		float distanceToPlayerSqr = glm::length2(currentChunkWorldPos - playerPosition);

		if (isChunkInView(distanceToPlayerSqr, farthestViewDistance, chunkDistanceToCorner, chunkLength, playerPosition, currentChunkWorldPos,
			rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal) == false) {
			continue;
		}

		chunks.push(currentChunkCoords + glm::ivec3( 0,  0, -1));
		chunks.push(currentChunkCoords + glm::ivec3( 0,  0,  1));
		chunks.push(currentChunkCoords + glm::ivec3( 1,  0,  0));
		chunks.push(currentChunkCoords + glm::ivec3(-1,  0,  0));
		chunks.push(currentChunkCoords + glm::ivec3( 0,  1,  0));
		chunks.push(currentChunkCoords + glm::ivec3( 0, -1,  0));


		// JUST FOR WATER
		if (currentChunkCoords.y == 0) {
			this->water->waterCoord(currentChunkCoords);
		}


		if (this->emptyChunks.find(currentChunkName) != this->emptyChunks.end()) continue;
		chunksToBeRendered.push_back(currentChunkName);
		nonEmptyChunksPos.push_back(currentChunkCoords);

		amountsOfChunks++;
	}
}

void CompleteMCManager::updateDetailedChunks(std::vector<std::string>& chunksToBeRendered, std::vector<glm::ivec3>& nonEmptyChunksPos, float chunkLength, int index, int LODIndex, float farthestViewDistanceFactor, int maxChunksForLOD, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal) {
	Camera* player = GameManager::getPlayer();
	glm::vec3 playerPosition = player->transform->getPosition();

	float chunkDistanceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
	// Since the view distance is how far away a plane is, we need the distance to the farthest point on that plane
	float farthestViewDistance = Camera::viewDistance * farthestViewDistanceFactor;


	glm::ivec3 margins[]{ glm::ivec3(-1, -1, -1), glm::ivec3( 0, -1, -1), glm::ivec3( 1, -1, -1), glm::ivec3(-1,  0, -1), glm::ivec3( 0,  0, -1), glm::ivec3( 1,  0, -1), glm::ivec3(-1,  1, -1), glm::ivec3( 0,  1, -1), glm::ivec3( 1,  1, -1), 
						  glm::ivec3(-1, -1,  0), glm::ivec3( 0, -1,  0), glm::ivec3( 1, -1,  0), glm::ivec3(-1,  0,  0), glm::ivec3( 0,  0,  0), glm::ivec3( 1,  0,  0), glm::ivec3(-1,  1,  0), glm::ivec3( 0,  1,  0), glm::ivec3( 1,  1,  0), 
						  glm::ivec3(-1, -1,  1), glm::ivec3( 0, -1,  1), glm::ivec3( 1, -1,  1), glm::ivec3(-1,  0,  1), glm::ivec3( 0,  0,  1), glm::ivec3( 1,  0,  1), glm::ivec3(-1,  1,  1), glm::ivec3( 0,  1,  1), glm::ivec3( 1,  1,  1), };

	std::unordered_set<std::string> visitedChunks;

	std::multimap<float, glm::ivec3> chunksOrderedByDistance;

	int currentSize = (int)nonEmptyChunksPos.size();
	while (index < currentSize) {
		glm::ivec3 baseChunkCoords = nonEmptyChunksPos[index] * 2;

		for (int i = 0; i < 27; i++) {
			glm::ivec3 currentChunkCoords = baseChunkCoords + margins[i];

			std::string currentChunkName = std::to_string(currentChunkCoords.x) + " " + std::to_string(currentChunkCoords.y) + " " + std::to_string(currentChunkCoords.z) + " " + std::to_string(LODIndex);
			if (this->emptyChunks.find(currentChunkName) != this->emptyChunks.end()) continue;
			
			if (visitedChunks.find(currentChunkName) != visitedChunks.end()) continue;
			visitedChunks.insert(currentChunkName);


			glm::vec3 currentChunkWorldPos = glm::vec3(currentChunkCoords) * chunkLength;
			float distanceToPlayerSqr = glm::length2(currentChunkWorldPos - playerPosition);

			if (isChunkInView(distanceToPlayerSqr, farthestViewDistance, chunkDistanceToCorner, chunkLength, playerPosition, currentChunkWorldPos,
				rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal) == false) {
				continue;
			}


			chunksOrderedByDistance.insert(std::make_pair(distanceToPlayerSqr, currentChunkCoords));
		}
		index++;
	}

	int amountsOfChunks = 0;
	for (auto pair : chunksOrderedByDistance) {
		if (amountsOfChunks >= maxChunksForLOD) break;

		nonEmptyChunksPos.push_back(pair.second);
		std::string currentChunkName = std::to_string(pair.second.x) + " " + std::to_string(pair.second.y) + " " + std::to_string(pair.second.z) + " " + std::to_string(LODIndex);
		chunksToBeRendered.push_back(currentChunkName);

		amountsOfChunks++;
	}
}

bool CompleteMCManager::isChunkInView(float distanceToPlayerSqr, float farthestViewDistance, float chunkDistanceToCorner, float chunkLength, const glm::vec3& playerPosition, const glm::vec3& currentChunkWorldPos, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal) {
	if (distanceToPlayerSqr > (farthestViewDistance + chunkDistanceToCorner) * (farthestViewDistance + chunkDistanceToCorner)) return false;

	// Check to see if possible that the chunk might be seen by the camera
	// check 1: behind
	glm::vec3 pointInView = playerPosition + -GameManager::getPlayer()->transform->getDirection() * glm::max(Camera::viewDistance, 1.0f);
	glm::vec3 positionInChunk = currentChunkWorldPos;
	positionInChunk.x = positionInChunk.x < pointInView.x ? glm::min(positionInChunk.x + chunkLength / 2.0f, pointInView.x) : glm::max(positionInChunk.x - chunkLength / 2.0f, pointInView.x);
	positionInChunk.y = positionInChunk.y < pointInView.y ? glm::min(positionInChunk.y + chunkLength / 2.0f, pointInView.y) : glm::max(positionInChunk.y - chunkLength / 2.0f, pointInView.y);
	positionInChunk.z = positionInChunk.z < pointInView.z ? glm::min(positionInChunk.z + chunkLength / 2.0f, pointInView.z) : glm::max(positionInChunk.z - chunkLength / 2.0f, pointInView.z);
	if (glm::dot(positionInChunk - playerPosition, -GameManager::getPlayer()->transform->getDirection()) <= 0.0f) {
		return false;
	}

	// check 2: View frustum culling (more expensive)
	glm::vec3 playerToChunkPos = currentChunkWorldPos - playerPosition;
	float rightProjectionDot = glm::dot(rightProjectionNormal, playerToChunkPos);
	float leftProjectionDot = glm::dot(leftProjectionNormal, playerToChunkPos);
	float upProjectionDot = glm::dot(upProjectionNormal, playerToChunkPos);
	float downProjectionDot = glm::dot(downProjectionNormal, playerToChunkPos);

	if (rightProjectionDot + chunkDistanceToCorner < 0.0f ||
		leftProjectionDot + chunkDistanceToCorner < 0.0f ||
		upProjectionDot + chunkDistanceToCorner < 0.0f ||
		downProjectionDot + chunkDistanceToCorner < 0.0f) {
		return false;
	}
	// -------------------------------------------------------------------

	return true;
}


bool CompleteMCManager::generateChunk(VertexBuffer* vertexBuffer, const std::string& name) {
	// Generate the noise
	int LOD = name.back() - '0';

	glm::vec3 position;
	std::istringstream ss(name);
	for (int i = 0; i < 3; i++) {
		int temp; ss >> temp;
		position[i] = temp * this->chunkLength * LOD;
	}
	
	this->noiseShaders->use();

	float cellLength = LOD * (this->chunkLength / this->cellsPerAxis);
	// Uniforms
	this->noiseShaders->setVec3("position", position);
	this->noiseShaders->setFloat("cellLength", cellLength);
	this->noiseShaders->setInt("LOD", LOD);

	// Compute shader
	glDispatchCompute((GLuint)(this->cellsPerAxis + 5), (GLuint)(this->cellsPerAxis + 5), (GLuint)(this->cellsPerAxis + 5));
	
	// -------------
	// Generate Mesh
	// -------------
	this->mcShaders->use();

	// Uniforms
	this->mcShaders->setFloat("cellLength", cellLength);
	this->mcShaders->setVec3("chunkPosition", position);

	// Reset counter
	unsigned int atomicCounterValue = 0u;
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &atomicCounterValue, GL_DYNAMIC_READ);

	// Make sure the noise has finished generating
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// Compute shader
	glDispatchCompute((GLuint)cellsPerAxis, (GLuint)cellsPerAxis, (GLuint)cellsPerAxis);


	// Make sure the marching cubes is finished
	//glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
	//glMemoryBarrier(GL_ALL_BARRIER_BITS);

	unsigned int amountOfVertices;
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned int), &amountOfVertices);
	amountOfVertices *= 3;

	if (amountOfVertices == 0) {
		return true;
	}

	// Copy data to an appropriately sized buffer
	GLint bufferSize = amountOfVertices * 6 * (int)sizeof(float);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
	glCopyNamedBufferSubData(this->writableVBO, vertexBuffer->VBO, 0, 0, bufferSize);

	vertexBuffer->amountOfVertices = amountOfVertices;
	vertexBuffer->LOD = LOD;
	vertexBuffer->timeOnCreation = glfwGetTime();

	return false;
}

void CompleteMCManager::render() {
	this->cubemap->draw(Camera::projectionMatrix, Camera::viewMatrix);

	// wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_CULL_FACE);

	this->renderingShaders->use();
	// directional light
	if (GameManager::getDirectionalLight()) {
		GameManager::getDirectionalLight()->setUniform(this->renderingShaders, Camera::viewMatrix);
	}
	else { // let the shader know that a directional light doesn't exist
		this->renderingShaders->setBool("directionalLight.exists", false);
	}

	// Renderer stuff
	// Uniforms
	this->renderingShaders->setMat4("view", Camera::viewMatrix);
	this->renderingShaders->setMat4("projection", Camera::projectionMatrix);
	this->renderingShaders->setFloat("viewDistance", Camera::viewDistance);
	this->renderingShaders->setVec3("playerPos", GameManager::getPlayer()->transform->getPosition());

	// material
	this->renderingShaders->setVec3("material.diffuse", this->material->diffuse);
	this->renderingShaders->setVec3("material.specular", this->material->specular);
	this->renderingShaders->setFloat("material.shininess", this->material->shininess);

	// textures
	glActiveTexture(GL_TEXTURE0);
	Engine::ResourceManager::getTexture("Mountain.jpg")->bind();
	glActiveTexture(GL_TEXTURE1);
	Engine::ResourceManager::getTexture("GrassEskilII.jpg")->bind();
	glActiveTexture(GL_TEXTURE2);
	Engine::ResourceManager::getTexture("Mountain.jpg")->bind();

	glActiveTexture(GL_TEXTURE6);
	Engine::ResourceManager::getTexture("SandEskil.jpg")->bind();

	for (auto pair : this->generatedChunks) {
		if (pair.first.back() == '4') {
			// Set LOD
			this->renderingShaders->setInt("LOD", 4);
			this->renderingShaders->setFloat("alpha", 1.0f);

			pair.second->draw();
		}
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	for (auto pair : this->generatedChunks) {
		if (pair.first.back() == '2') {
			// Set LOD
			this->renderingShaders->setInt("LOD", 2);
			this->renderingShaders->setFloat("alpha", glm::min(1.0f, (float)(glfwGetTime() - pair.second->timeOnCreation) * 2.0f));

			pair.second->draw();
		}
	}
	glClear(GL_DEPTH_BUFFER_BIT);
	for (auto pair : this->generatedChunks) {
		if (pair.first.back() == '1') {
			// Set LOD
			this->renderingShaders->setInt("LOD", 1);
			this->renderingShaders->setFloat("alpha", glm::min(1.0f, (float)(glfwGetTime() - pair.second->timeOnCreation) * 2.0f));

			pair.second->draw();
		}
	}

	// Water will only work if there is one LOD anyway
	this->water->draw(this->chunkLength, this->chunkLength / (float)this->cellsPerAxis);

	glBindVertexArray(0);
	this->renderingShaders->unbind();

	// wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_CULL_FACE);

	//regenerateChunks();
}

void CompleteMCManager::renderImGui() {
	// Chunk settings
	{
		static float verticalFov = Camera::verticalFov;
		static float viewDistanceSlider = Camera::viewDistance;

		static int cellsPerAxisSlider = this->cellsPerAxis;
		static int oldCellsPerAxisSlider = cellsPerAxisSlider;

		static float chunkLengthSlider = this->chunkLength;
		static float oldChunkLengthSlider = chunkLengthSlider;

		static bool keepCellRatio = true;
		static float ratio = chunkLengthSlider / cellsPerAxisSlider;
		static bool oldKeepCellRatio = keepCellRatio;

		static int vertexBuffersSlider = this->numberOfVertexBuffers;
		static int oldVertexBuffersSlider = vertexBuffersSlider;

		ImGui::Begin("Chunk Settings");

		ImGui::SliderFloat("Vertical FOV", &verticalFov, 1.0f, 179.0f);
		float aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
		ImGui::Text("Horizontal Fov: %.3f", glm::degrees(2.0f * glm::atan(glm::tan(glm::radians(Camera::verticalFov) * 0.5f) * aspectRatio)));

		ImGui::SliderFloat("View distance", &viewDistanceSlider, 1.0f, 500.0f);
		ImGui::SliderFloat("Chunk length", &chunkLengthSlider, 1.0f, 20.0f);
		ImGui::SliderInt("Cells per axis", &cellsPerAxisSlider, 1, 100);
		ImGui::Checkbox("Keep ratio", &keepCellRatio);

		ImGui::SliderInt("Vertex Buffers", &vertexBuffersSlider, 1, 1000);

		ImGui::Text(("Visible chunks: " + std::to_string(this->generatedChunks.size())).c_str());

		ImGui::End();


		Camera::verticalFov = verticalFov;
		Camera::viewDistance = viewDistanceSlider; this->viewDistanceSqrd = viewDistanceSlider * viewDistanceSlider;

		// Update cells if settings were changed
		if (cellsPerAxisSlider != oldCellsPerAxisSlider || chunkLengthSlider != oldChunkLengthSlider || vertexBuffersSlider != oldVertexBuffersSlider) {
			if (keepCellRatio) {
				if (cellsPerAxisSlider != oldCellsPerAxisSlider) {
					chunkLengthSlider = glm::clamp(cellsPerAxisSlider * ratio, 1.0f, 100.0f);
				}
				else {
					cellsPerAxisSlider = glm::clamp((int)(chunkLengthSlider / ratio), 1, 100);
				}
			}

			this->chunkLength = chunkLengthSlider;
			this->cellsPerAxis = cellsPerAxisSlider;

			this->chunkDistanceToCorner = glm::sqrt((this->chunkLength / 2.0f) * (this->chunkLength / 2.0f) + (this->chunkLength / 2.0f) * (this->chunkLength / 2.0f) + (this->chunkLength / 2.0f) * (this->chunkLength / 2.0f));
			this->chunkDistanceToCornerSqrd = this->chunkDistanceToCorner * this->chunkDistanceToCorner;

			this->numberOfVertexBuffers = vertexBuffersSlider;

			// Now we need to regenerate the chunks
			regenerateChunks();

			oldCellsPerAxisSlider = cellsPerAxisSlider;
			oldChunkLengthSlider = chunkLengthSlider;
			oldVertexBuffersSlider = vertexBuffersSlider;
		}

		// Update ratio if keepRatio was turned on
		if (keepCellRatio != oldKeepCellRatio) {
			if (keepCellRatio) {
				ratio = chunkLengthSlider / cellsPerAxisSlider;
			}
			oldKeepCellRatio = keepCellRatio;
		}
	}
	// Noise settings
	{
		ImGui::Begin("Noise Settings");

		static int seedSlider = this->seed;
		static int oldSeedSlider = seedSlider;

		ImGui::SliderInt("Seed", &seedSlider, 0, 1000000);

		glm::ivec3 playerPos = glm::round(GameManager::getPlayer()->transform->getPosition());
		ImGui::Text(("Coordinates: (" + std::to_string(playerPos.x) + ", " + std::to_string(playerPos.y) + ", " + std::to_string(playerPos.z) + ")").c_str());

		ImGui::End();

		if (oldSeedSlider != seedSlider) {
			this->seed = (unsigned int)seedSlider;

			// Now we need to regenerate the chunks
			regenerateChunks();

			oldSeedSlider = seedSlider;
		}
	}
}
