#pragma once

namespace Engine { 
	class Shader; 
	class Material;
}

#include "Renderer.h"

#include "glm/glm.hpp"

#include <vector>
#include <string>

namespace Engine {
	class MeshRenderer : public Renderer {
	public:
		MeshRenderer(const std::vector<std::string>& shaderFiles, const std::string& shaderProgramName, const std::vector<glm::vec3>& _vertexArray, Material* _material = nullptr);
		~MeshRenderer();

		void setupVertexArrayObject() override;

		void render(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat3& normalMatrix) override;
	};
}
