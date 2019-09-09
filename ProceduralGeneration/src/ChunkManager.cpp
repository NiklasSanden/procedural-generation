#include "ChunkManager.h"

#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "engine/Game.h"
#include "GameManager.h"
#include "Camera.h"
#include "Chunk.h"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

ChunkManager::ChunkManager(const std::string& name) : GameObject(name) {
	
}

ChunkManager::~ChunkManager() {

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
	if (oldLoadedChunks.size() > 0)
		GameManager::getGamePtr()->deleteGameObjects(oldLoadedChunks);
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
