#pragma once

#include "engine/GameObject.h"

#include <string>

namespace ProceduralGeneration {
	class Cube : public Engine::GameObject {
	public:
		Cube(std::string name);
		~Cube() {};

		void awake() override;
		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
	};
}
