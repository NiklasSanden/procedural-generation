#pragma once

#include "engine/GameObject.h"

#include "glm/glm.hpp"

#include <unordered_map>
#include <string>

namespace ProceduralGeneration {
	class ChunkManager : public Engine::GameObject {
	public:
		// Settings
		float chunkLength = 20.0f;
		float viewDistance = 70.0f;
		float updateChunksPlayerPositionThreshold = 1.0f;
		// Variables from settings
		float chunkDistaceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
		float chunkDistanceToCornerSqrd = chunkDistaceToCorner * chunkDistaceToCorner;
		float viewDistanceSqrd = viewDistance * viewDistance;
		float updateChunksPlayerPositionThresholdSqrd = updateChunksPlayerPositionThreshold * updateChunksPlayerPositionThreshold;
	private:
		std::unordered_map<std::string, std::string> loadedChunks;
		glm::vec3 oldPlayerPosition = glm::vec3(0.0f);
	public:
		ChunkManager(const std::string& name);
		~ChunkManager();

		void update(float deltaTime) override;
		void renderImGui() override {};
	private:
		std::string vec3ToString(const glm::vec3& vector);

		int chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector);
	};
}
