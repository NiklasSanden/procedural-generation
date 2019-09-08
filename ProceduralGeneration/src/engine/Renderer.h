#pragma once

namespace Engine {
	class Shader;
	class Material;
	class ResourceManager;
}

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace Engine {
	class Renderer {
	public:
		Shader* shaderProgram = nullptr;
		Material* material; // Default value in constructor

		std::vector<std::string> textureNames;

		std::vector<glm::vec3> vertexArray;
		std::vector<glm::vec3> normalArray;
		std::vector<glm::vec2> texCoordArray;
		std::vector<unsigned int> indexArray;
		
		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;
	public:
		Renderer(const std::vector<std::string>& shaderFiles, const std::string& shaderProgramName, const std::vector<glm::vec3>& _vertexArray, Material* _material = nullptr);
		virtual ~Renderer(); // Does not need to delete shaderProgram as ResourceManager does that

		virtual void setMaterial(Material* _material);
		virtual void setVertexArray(const std::vector<glm::vec3>& _vertexArray);
		virtual void setNormalArray(const std::vector<glm::vec3>& _normalArray);
		virtual void setTexCoordArray(const std::vector<glm::vec2>& _texCoordArray);
		virtual void setIndexArray(const std::vector<unsigned int>& _indexArray);
		
		virtual void addTextureName(const std::string& textureName);

		virtual void calculateNormals();

		// setupVertexArrayObject and render are up to the child class to override
		virtual void setupVertexArrayObject() {};

		virtual void render(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat3& normalMatrix) {};

	protected:
		int getStrideValue();

		void* getVertexArrayOffsetPtr();

		void* getNormalArrayOffsetPtr();

		void* getTexCoordArrayOffsetPtr();
	};
}
