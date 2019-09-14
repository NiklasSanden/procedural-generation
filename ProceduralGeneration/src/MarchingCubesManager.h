#pragma once

namespace Engine {
	class Shader;
	class Material;
}

#include "engine/GameObject.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace ProceduralGeneration {
	class MarchingCubesManager : public Engine::GameObject {
	public:
		// Settings
		float chunkLength = 9.0f;
		int cellsPerAxis = 3;
		float viewDistance = 50.0f;
		float updateChunksPlayerPositionThreshold = 1.0f;
		// Variables from settings
		float chunkDistaceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
		float chunkDistanceToCornerSqrd = chunkDistaceToCorner * chunkDistaceToCorner;

		float viewDistanceSqrd = viewDistance * viewDistance;

		float updateChunksPlayerPositionThresholdSqrd = updateChunksPlayerPositionThreshold * updateChunksPlayerPositionThreshold;

	private:
		std::vector<float> chunkPositionVectors;
		std::vector<float> cellPositionVectors;
		glm::vec3 oldPlayerPosition = glm::vec3(0.0f);

		Engine::Shader* shaderProgram = nullptr;
		Engine::Material* material = nullptr;

		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int instancedVBO = 0;
	public:
		MarchingCubesManager(const std::string& name);
		~MarchingCubesManager();

		void update(float deltaTime) override;
		void render() override;
		void renderImGui() override;
	private:
		int chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector, const glm::vec3& playerPosition);

		void calculateCellPositions();
	};
}
