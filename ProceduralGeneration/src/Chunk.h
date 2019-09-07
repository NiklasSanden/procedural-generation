#pragma once

#include "engine/GameObject.h"

namespace ProceduralGeneration {
	class Chunk : public Engine::GameObject {
	private:
	public:
		Chunk(std::string& name);
		~Chunk();

		void awake() override;
		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
		void renderImGui() override;
	};
}
