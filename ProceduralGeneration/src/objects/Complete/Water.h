#pragma once

namespace Engine {
	class Shader;
	class Material;
	class Texture2D;
}

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <stack>
#include <tuple>

namespace ProceduralGeneration {
	class Water {
	private:
		Engine::Texture2D* waterTexture;
		std::vector<std::pair<std::string, glm::ivec2>> waterToDraw;

		Engine::Shader* renderingShaders = nullptr;
		Engine::Shader* noiseShaders = nullptr;

		Engine::Material* material = nullptr;

		unsigned int noiseTexture = 0;

		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;

		unsigned int amountOfIndices = 0;
		int pointsPerAxis = 0;
	public:
		Water(int pointsPerAxis);
		~Water();

		void regenerate(int pointsPerAxis);
		void waterCoord(const glm::ivec3& coord);
		void draw(float chunkLength, float cellLength);
	};
}
