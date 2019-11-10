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
	class Cubemap {
	private:
		Engine::Shader* renderingShaders = nullptr;

		unsigned int VAO = 0;
		unsigned int VBO = 0;
	
		unsigned int cubemapTexture = 0;
	public:
		Cubemap();
		~Cubemap();

		void draw(const glm::mat4& proj, const glm::mat4& view);
		void bind();
	};
}
