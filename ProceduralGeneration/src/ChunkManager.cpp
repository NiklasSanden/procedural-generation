#include "ChunkManager.h"

#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "engine/Game.h"
#include "engine/Lights.h"
#include "engine/Shader.h"
#include "engine/Material.h"
#include "engine/ResourceManager.h"
#include "GameManager.h"
#include "Camera.h"
#include "Chunk.h"

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

ChunkManager::ChunkManager(const std::string& name) : GameObject(name) {
	this->shaderProgram = Engine::ResourceManager::createShaderProgram({ "chunk.vert", "chunk.frag" }, "ChunkShader");
	this->material = new Engine::Material();

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
}

ChunkManager::~ChunkManager() {
	delete this->material;

	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void ChunkManager::update(float deltaTime) {
	Camera* player = GameManager::getPlayer();

	glm::vec3 playerPosition = player->transform->getPosition();

	if (glm::length2(this->oldPlayerPosition - playerPosition) < this->updateChunksPlayerPositionThresholdSqrd) {
		return;
	}
	this->oldPlayerPosition = playerPosition;

	glm::vec3 roundedPlayerPosition = glm::round(playerPosition / this->chunkLength);

	// This will keep track of all the old loaded chunks, and in the loop will remove all that should still be rendered. The once remaining will be removed
	std::unordered_set<std::string> oldLoadedChunks;
	for (auto& chunk : loadedChunks) {
		oldLoadedChunks.insert(chunk.second);
	}

	// Notice that left, down and front are negative. That is because they will be used in the for loop, and will be added to the roundedPlayerPosition
	int chunksInRangeUp = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(0.0f, 1.0f, 0.0f));
	int chunksInRangeDown = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(0.0f, -1.0f, 0.0f));
	int chunksInRangeRight = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(1.0f, 0.0f, 0.0f));
	int chunksInRangeLeft = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(-1.0f, 0.0f, 0.0f));
	int chunksInRangeFront = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(-1.0f, 0.0f, 0.0f));
	int chunksInRangeBack = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(1.0f, 0.0f, 0.0f));

	// Loop through all of the chunks that could be in range
	for (int y = chunksInRangeDown; y <= chunksInRangeUp; y++) {
		for (int x = chunksInRangeLeft; x <= chunksInRangeUp; x++) {
			for (int z = chunksInRangeFront; z <= chunksInRangeBack; z++) {
				
				glm::vec3 currentChunk = glm::vec3(roundedPlayerPosition.x + x, roundedPlayerPosition.y + y, roundedPlayerPosition.z + z);
				std::string currentChunkPosition = vec3ToString(currentChunk);
				std::string chunkName = "Chunk: " + currentChunkPosition;
				glm::vec3 asd = currentChunk * this->chunkLength;

				if (glm::length2((currentChunk - roundedPlayerPosition) * this->chunkLength) <= (this->viewDistance + this->chunkDistaceToCorner) * (this->viewDistance + this->chunkDistaceToCorner)) {
					if (this->loadedChunks.find(currentChunkPosition) == this->loadedChunks.end()) {
						Chunk* chunk = new Chunk(chunkName, currentChunk * this->chunkLength, this);
						addChunk(chunk);
						GameManager::getGamePtr()->addGameObject(chunk);
						this->loadedChunks[currentChunkPosition] = chunkName;
					}
					else {
						// Remove from oldLoadedChunks
						auto& it = oldLoadedChunks.find(chunkName);
						if (it != oldLoadedChunks.end()) {
							oldLoadedChunks.erase(it);
						}
					}
				}
				else {
					if (this->loadedChunks.find(currentChunkPosition) != this->loadedChunks.end()) {
						this->loadedChunks.erase(currentChunkPosition);
					}
				}
			}
		}
	}

	// Delete the chunks
	if (oldLoadedChunks.size() > 0) {
		deleteChunks(oldLoadedChunks);
		GameManager::getGamePtr()->deleteGameObjects(oldLoadedChunks);
	}
}

std::string ChunkManager::vec3ToString(const glm::vec3& vector) {
	return "(" + std::to_string((int)std::roundf(vector.x)) + ", " + std::to_string((int)std::roundf(vector.y)) + ", " + std::to_string((int)std::roundf(vector.z)) + ")";
}

int ChunkManager::chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector) {
	glm::vec3 currentPosition = startPosition + incrementVector;
	
	int answer = 0;
	while (true) {
		if (glm::length2((currentPosition - startPosition) * this->chunkLength) <= this->viewDistanceSqrd) {
			answer++;
			currentPosition += incrementVector;
		}
		else {
			break;
		}
	}

	return answer;
}

void ChunkManager::addChunk(Chunk* chunk) {
	this->loadedChunksPtr.push_back(chunk);
}

void ChunkManager::deleteChunks(const std::unordered_set<std::string>& chunkNames) {
	std::vector<Chunk*> newChunkList;

	for (int i = 0; i < this->loadedChunksPtr.size(); i++) {
		if (chunkNames.find(this->loadedChunksPtr[i]->name) == chunkNames.end()) {
			newChunkList.push_back(this->loadedChunksPtr[i]);
		}
	}

	this->loadedChunksPtr = newChunkList;
}

void ChunkManager::render() {
	// used in multiple function calls
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	this->shaderProgram->use();
	// directional light
	if (GameManager::getDirectionalLight()) {
		GameManager::getDirectionalLight()->setUniform(this->shaderProgram, Camera::viewMatrix);
	}
	else { // let the shader know that a directional light doesn't exist
		this->shaderProgram->setBool("directionalLight.exists", false);
	}

	
	// Renderer stuff
	// Uniforms
	this->shaderProgram->use();
	this->shaderProgram->setMat4("model", modelMatrix);
	this->shaderProgram->setMat4("view", Camera::viewMatrix);
	this->shaderProgram->setMat4("projection", Camera::projectionMatrix);
	this->shaderProgram->setMat3("normal", this->transform->getNormalMatrix(glm::mat4(1.0f), Camera::viewMatrix));
	// material
	this->shaderProgram->setVec3("material.diffuse", this->material->diffuse);
	this->shaderProgram->setVec3("material.specular", this->material->specular);
	this->shaderProgram->setFloat("material.shininess", this->material->shininess);

	
	// Setup VBO and EBO for all chunks
	std::vector<float> vertexAttributes;
	std::vector<unsigned int> allIndicesArray;
	int amountOfVertices = 0;
	for (auto& chunk : this->loadedChunksPtr) {
		std::vector<glm::vec3>* vertexArray;
		std::vector<unsigned int>* indexArray;
		std::vector<glm::vec3>* normalArray;
		chunk->getArraysForRendering(vertexArray, indexArray, normalArray);

		if (vertexArray == nullptr) continue;

		if (vertexArray->size() != normalArray->size()) {
			std::cout << "---------> Error - VertexArray doesn't have the same size as normalArray in ChunkManager" << std::endl;
			return;
		}

		allIndicesArray.reserve(allIndicesArray.size() + indexArray->size());
		for (int i = 0; i < indexArray->size(); i++) {
			allIndicesArray.push_back((*indexArray)[i] + amountOfVertices);
		}

		amountOfVertices += (int)vertexArray->size();

		for (unsigned int i = 0; i < vertexArray->size(); i++) {
			vertexAttributes.push_back((*vertexArray)[i].x);
			vertexAttributes.push_back((*vertexArray)[i].y);
			vertexAttributes.push_back((*vertexArray)[i].z);

			vertexAttributes.push_back((*normalArray)[i].x);
			vertexAttributes.push_back((*normalArray)[i].y);
			vertexAttributes.push_back((*normalArray)[i].z);
		}
	}

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexAttributes.size() * sizeof(float), vertexAttributes.data(), GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// index array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndicesArray.size() * sizeof(unsigned int), allIndicesArray.data(), GL_DYNAMIC_DRAW);


	// Render all chunks
	glDrawElements(GL_TRIANGLES, (GLsizei)allIndicesArray.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	this->shaderProgram->unbind();
}
