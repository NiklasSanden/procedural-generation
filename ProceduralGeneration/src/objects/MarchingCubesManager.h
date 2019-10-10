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
#include <set>
#include <unordered_map>

namespace ProceduralGeneration {
	struct MarchingCubesChunk {
	public:
		std::string name;
		glm::vec3 position;
		float distanceToPlayer;
		MarchingCubesChunk(const std::string& _name, const glm::vec3& _position) : name(_name), position(_position), distanceToPlayer(0.0f) {}
	};

	class MarchingCubesManager : public Engine::GameObject {
	public:
		// Settings
		float chunkLength = 9.0f;
		int cellsPerAxis = 3;
		// Variables from settings
		float chunkDistanceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
		float chunkDistanceToCornerSqrd = chunkDistanceToCorner * chunkDistanceToCorner;

		float viewDistanceSqrd = 0.0f;

	private:
		std::vector<MarchingCubesChunk> activeChunks;
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
		int chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector, const glm::vec3& playerPosition, float farthestViewDistance);

		void calculateCellPositions();
	};
}
