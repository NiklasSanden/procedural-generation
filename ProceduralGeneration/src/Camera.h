#pragma once

#include "engine/GameObject.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace ProceduralGeneration {
	class Camera : public Engine::GameObject {
	// static
	public:
		static glm::mat4 viewMatrix;
		static glm::mat4 projectionMatrix;
	// instance
	private:
		// settings
		float movementSpeed = 3.0f;
		float anglesRotatedX = 0.0f;
	public:
		Camera();
		~Camera() {};

		void awake() override;
		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
	};
}
