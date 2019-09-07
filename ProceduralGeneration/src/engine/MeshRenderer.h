#pragma once

namespace Engine { class Shader; }

#include "Renderer.h"

#include "glm/glm.hpp"

#include <vector>
#include <string>

namespace Engine {
	class MeshRenderer : public Renderer {
	public:
		MeshRenderer(std::vector<std::string>& shaderFiles, std::string& shaderProgramName, std::vector<glm::vec3>& _vertexArray);
		~MeshRenderer();

		void setupVertexArrayObject() override;

		void render(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix) override;
	};
}
