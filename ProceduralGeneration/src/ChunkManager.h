#pragma once

namespace Engine {
	class Shader;
	class Material;
}

namespace ProceduralGeneration {
	class Chunk;
}

#include "engine/GameObject.h"

#include "glm/glm.hpp"

#include <unordered_map>
#include <unordered_set>
#include <string>

namespace ProceduralGeneration {
	class ChunkManager : public Engine::GameObject {
	public:
		// Settings
		float chunkLength = 20.0f;
		float viewDistance = 100.0f;
		float updateChunksPlayerPositionThreshold = 1.0f;
		// Variables from settings
		float chunkDistaceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
		float chunkDistanceToCornerSqrd = chunkDistaceToCorner * chunkDistaceToCorner;
		float viewDistanceSqrd = viewDistance * viewDistance;
		float updateChunksPlayerPositionThresholdSqrd = updateChunksPlayerPositionThreshold * updateChunksPlayerPositionThreshold;
	private:
		std::vector<Chunk*> loadedChunksPtr;
		std::unordered_map<std::string, std::string> loadedChunks;
		glm::vec3 oldPlayerPosition = glm::vec3(0.0f);

		Engine::Shader* shaderProgram = nullptr;
		Engine::Material* material = nullptr;

		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;
	public:
		ChunkManager(const std::string& name);
		~ChunkManager();

		void update(float deltaTime) override;
		void render() override;
		void renderImGui() override;
	private:
		std::string vec3ToString(const glm::vec3& vector);

		int chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector);
	
		void addChunk(Chunk* chunk);
		void deleteChunks(const std::unordered_set<std::string>& chunkNames);
	};
}
