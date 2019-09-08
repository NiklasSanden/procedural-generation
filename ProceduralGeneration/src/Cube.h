#pragma once

#include "engine/GameObject.h"

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
