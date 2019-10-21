#pragma once

namespace Engine {
	class Shader;
	class Material;
}
namespace ProceduralGeneration {
	class CPUMarchingCubesChunk;
}

#include "engine/objects/GameObject.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace ProceduralGeneration {
	class CPUMarchingCubesManager : public Engine::GameObject {
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
		std::multimap<float, CPUMarchingCubesChunk*> activeChunks;
		std::unordered_map<std::string, CPUMarchingCubesChunk*> generatedChunks;

		Engine::Shader* shaderProgram = nullptr;
		Engine::Material* material = nullptr;
	public:
		CPUMarchingCubesManager(const std::string& name);
		~CPUMarchingCubesManager();

		void update(float deltaTime) override;
		void render() override;
		void renderImGui() override;
	private:
		int chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector, const glm::vec3& playerPosition, float farthestViewDistance);

		void regenerateChunks();
	};
}
