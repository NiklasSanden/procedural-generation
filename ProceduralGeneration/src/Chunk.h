#pragma once

#include "engine/GameObject.h"

namespace ProceduralGeneration {
	class Chunk : public Engine::GameObject {
	private:
		bool doneGeneratingChunk = false;

		void generateChunk();
	public:
		Chunk(const std::string& name);
		~Chunk();

		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
		void renderImGui() override;
	};
}
