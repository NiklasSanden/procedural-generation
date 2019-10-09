#pragma once

namespace Engine {
	class Shader;
	class Material;
}
namespace ProceduralGeneration {
	class NoiseTexture3D;
}

#include "engine/objects/GameObject.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace ProceduralGeneration {
	class MarchingCubesManager : public Engine::GameObject {
	public:
		// Settings
		float chunkLength = 9.0f;
		int cellsPerAxis = 3;
		// Variables from settings
		float chunkDistaceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
		float chunkDistanceToCornerSqrd = chunkDistaceToCorner * chunkDistaceToCorner;

		float viewDistanceSqrd = 0.0f;

	private:
		std::vector<glm::vec3> chunkPositionVectors;
		std::vector<float> cellPositionVectors;
		std::unordered_map<std::string, NoiseTexture3D*> noiseTextures3D;

		Engine::Shader* shaderProgram = nullptr;
		Engine::Material* material = nullptr;

		unsigned int VAO = 0;
		unsigned int VBO = 0;
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
