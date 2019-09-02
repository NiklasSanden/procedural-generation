#pragma once

#include "engine/GameObject.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace ProceduralGeneration {
	class Camera : public Engine::GameObject {
	public:
		Camera() : GameObject("Camera") {};
		~Camera() {};

		void awake() override;
		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
	};
}
