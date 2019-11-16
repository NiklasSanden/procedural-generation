#pragma once

namespace Engine {
	class Shader;
	class Material;
}
namespace ProceduralGeneration {
	struct VertexBuffer;
	struct VBOManager;
}

#include "engine/objects/GameObject.h"
#include "Water.h"
#include "Cubemap.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <stack>

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

		unsigned int numberOfVertexBuffers = 300;

	private:
		Water* water = nullptr;
		Cubemap* cubemap = nullptr;

		std::unordered_map<std::string, VertexBuffer*> generatedChunks;
		std::unordered_set<std::string> emptyChunks;

		Engine::Material* material = nullptr;
		Engine::Shader* renderingShaders = nullptr;
		Engine::Shader* noiseShaders = nullptr;
		Engine::Shader* mcShaders = nullptr;

		VBOManager* vboManager = nullptr;

		unsigned int noiseTextureID;
		unsigned int preCalculatedNoiseTextureID;
		unsigned int writableVBO;
		unsigned int atomicCounter;
	public:
		CompleteMCManager(const std::string& name);
		~CompleteMCManager();

		void update(float deltaTime) override;
		void render() override;
		void renderImGui() override;
	private:
		void updateActiveChunks(std::vector<std::string>& chunksToBeRendered, std::vector<glm::ivec3>& chunksToBeRenderedPos, float chunkLength, int LODIndex, float farthestViewDistanceFactor, int maxChunksForLOD, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal);
		void updateDetailedChunks(std::vector<std::string>& chunksToBeRendered, std::vector<glm::ivec3>& nonEmptyChunksPos, float chunkLength, int index, int LODIndex, float farthestViewDistanceFactor, int maxChunksForLOD, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal);
		bool isChunkInView(float distanceToPlayerSqr, float farthestViewDistance, float chunkDistanceToCorner, float chunkLength, const glm::vec3& playerPosition, const glm::vec3& currentChunkWorldPos, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal);
		bool generateChunk(VertexBuffer* vertexBuffer, const std::string& name);

		void regenerateChunks();
		void getPrecalculatedNoiseDate(std::vector<float>& preCalculatedNoiseData);
	};
}
