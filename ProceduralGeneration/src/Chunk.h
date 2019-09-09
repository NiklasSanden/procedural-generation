#pragma once

namespace ProceduralGeneration {
	class ChunkManager;
}

#include "engine/GameObject.h"

#include "glm/glm.hpp"

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
		void render() override;
		void renderImGui() override;
	};
}
