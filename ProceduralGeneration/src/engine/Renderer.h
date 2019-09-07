#pragma once

#include "Shader.h"
#include "ResourceManager.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace Engine {
	class Renderer {
	public:
		Shader* shaderProgram = nullptr;
		
		std::vector<std::string> textureNames;

		std::vector<glm::vec3> vertexArray;
		std::vector<glm::vec3> normalArray;
		std::vector<glm::vec2> texCoordArray;
		std::vector<unsigned int> indexArray;
		
		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;
	public:
		Renderer(std::vector<std::string>& shaderFiles, std::string& shaderProgramName, std::vector<glm::vec3>& _vertexArray) {
			this->shaderProgram = Engine::ResourceManager::createShaderProgram(shaderFiles, shaderProgramName);
			this->vertexArray = _vertexArray;
		};

		virtual ~Renderer() { // Does not need to delete shaderProgram as ResourceManager does that
			if (VAO != 0) glDeleteVertexArrays(1, &VAO);
			if (VBO != 0) glDeleteBuffers(1, &VBO);
			if (EBO != 0) glDeleteBuffers(1, &EBO);
		} 

		virtual void setVertexArray(std::vector<glm::vec3>& _vertexArray) {
			vertexArray = _vertexArray;
		}
		virtual void setNormalArray(std::vector<glm::vec3>& _normalArray) {
			normalArray = _normalArray;
		}
		virtual void setTexCoordArray(std::vector<glm::vec2>& _texCoordArray) {
			texCoordArray = _texCoordArray;
		}
		virtual void setIndexArray(std::vector<unsigned int>& _indexArray) {
			indexArray = _indexArray;
		}
		virtual void addTextureName(std::string& textureName) {
			textureNames.push_back(textureName);
		}

		virtual void setupVertexArrayObject() {};

		virtual void render(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix) {};

	protected:
		int getStrideValue() {
			int amountOfFloats = 3; // vertex array is never empty
			
			if (this->normalArray.size() > 0)
				amountOfFloats += 3;

			if (this->texCoordArray.size() > 0)
				amountOfFloats += 2;

			return amountOfFloats * sizeof(float);
		}

		void* getVertexArrayOffsetPtr() {
			return (void*)0;
		}

		void* getNormalArrayOffsetPtr() {
			return (void*)(3 * sizeof(float));  // vertex array is never empty
		}

		void* getTexCoordArrayOffsetPtr() {
			int amountOfFloats = 3; //  vertex array is never empty

			if (this->normalArray.size() > 0)
				amountOfFloats += 3;

			return (void*)(amountOfFloats * sizeof(float));
		}
	};
}
