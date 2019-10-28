#pragma once

namespace ProceduralGeneration {
	class ChunkManager;
}

#include "engine/objects/GameObject.h"

#include "glm/glm.hpp"

#include <vector>

namespace ProceduralGeneration {
	class Chunk : public Engine::GameObject {
	private:
		ChunkManager* chunkManager = nullptr;
		bool doneGeneratingChunk = false;
		std::vector<float> vertexAttributes;

		void generateChunk();
	public:
		Chunk(const std::string& name, const glm::vec3& position, ChunkManager* _chunkManager);
		~Chunk();

		void update(float deltaTime) override;
		void fixedUpdate() override;

		// 
		void getArraysForRendering(std::vector<float>*& _vertexAttributes, std::vector<unsigned int>*& _indexArray);
	};
}
