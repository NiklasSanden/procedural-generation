#pragma once

#include "engine/GameObject.h"

#include "glm/glm.hpp"

#include <unordered_map>
#include <string>

namespace ProceduralGeneration {
	class ChunkManager : public Engine::GameObject {
	public:
		// Settings
		float chunkLength = 10.0f;
		float viewDistanceSqrd = 450.0f;
		float updateChunksPlayerPositionThresholdSqrd = 1.0f;
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
