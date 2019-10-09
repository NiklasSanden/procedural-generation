#pragma once

#include "engine/objects/GameObject.h"

namespace ProceduralGeneration {
	class Cube : public Engine::GameObject {
	public:
		Cube(const std::string& name);
		~Cube();

		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
	};
}
