#pragma once

#include "engine/GameObject.h"

#include "glm/glm.hpp"

#include <vector>

namespace ProceduralGeneration {
	class Camera : public Engine::GameObject {
	// static
	public:
		static glm::mat4 viewMatrix;
		static glm::mat4 projectionMatrix;
		static float verticalFov;
	// instance
	private:
		// settings
		float movementSpeed = 10.0f;
		float anglesRotatedX = 0.0f;
	public:
		Camera(const std::string& name);
		~Camera() {};

		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
	};
}
