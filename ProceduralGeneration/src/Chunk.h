#pragma once

namespace ProceduralGeneration {
	class ChunkManager;
}

#include "engine/GameObject.h"

#include "glm/glm.hpp"

#include <vector>

namespace ProceduralGeneration {
	class Chunk : public Engine::GameObject {
	private:
		ChunkManager* chunkManager = nullptr;
		bool doneGeneratingChunk = false;

		void generateChunk();
	public:
		Chunk(const std::string& name, const glm::vec3& position, ChunkManager* _chunkManager);
		~Chunk();

		void update(float deltaTime) override;
		void fixedUpdate() override;

		// 
		void getArraysForRendering(std::vector<glm::vec3>*& _vertexArray, std::vector<unsigned int>*& _indexArray, std::vector<glm::vec3>*& _normalArray);
	};
}
