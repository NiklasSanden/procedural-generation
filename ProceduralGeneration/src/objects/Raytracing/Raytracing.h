#pragma once

namespace Engine {
	class Shader;
	class Material;
}

#include "engine/objects/GameObject.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>

namespace ProceduralGeneration {
	class Raytracing : public Engine::GameObject {
	private:
		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;

		// Settings
		float stepLength = 10.0f;
		unsigned int seed = 0;

		int width = 100;
		int height = 100;

		float viewDistance = 0.0f;
		float viewDistanceSqrd = 0.0f;

		unsigned int rayTexture;
		unsigned int renderedTexture;

	private:
		Engine::Shader* shaderProgram = nullptr;
		Engine::Shader* computeShader = nullptr;
		Engine::Material* material = nullptr;
	public:
		Raytracing(const std::string& name);
		~Raytracing();

		void update(float deltaTime) override;
		void updateRayTexture();
		void render() override;
		void renderImGui() override;
	};
}
