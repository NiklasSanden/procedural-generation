#pragma once

namespace Engine {
	class Shader;
	class Material;
}

#include "engine/objects/GameObject.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>

namespace ProceduralGeneration {
	class CompleteMCManager : public Engine::GameObject {
	public:
		// Settings
		float chunkLength = 9.0f;
		int cellsPerAxis = 9;
		unsigned int seed = 0;
		// Variables from settings
		float chunkDistanceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
		float chunkDistanceToCornerSqrd = chunkDistanceToCorner * chunkDistanceToCorner;

		float viewDistanceSqrd = 0.0f;

	private:
		std::multimap<float, ComputeMCChunk*> activeChunks;
		std::unordered_map<std::string, ComputeMCChunk*> generatedChunks;
		std::unordered_set<std::string> emptyChunks;
		std::unordered_set<std::string> oldActiveChunks;

		Engine::Shader* shaderProgram = nullptr;
		Engine::Material* material = nullptr;
	public:
		CompleteMCManager(const std::string& name);
		~CompleteMCManager();

		void update(float deltaTime) override;
		void render() override;
		void renderImGui() override;
	private:
		void updateActiveChunks(float chunkLength, int LODIndex, float viewDistance, float farthestViewDistanceFactor, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal);
		int chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector, const glm::vec3& playerPosition, float farthestViewDistance, float chunkLength, float chunkDistanceToCorner);

		void regenerateChunks();
	};
}
