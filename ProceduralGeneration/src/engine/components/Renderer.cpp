#include "engine/components/Renderer.h"

#include "engine/resources/Shader.h"
#include "engine/resources/ResourceManager.h"
#include "engine/resources/Material.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <vector>
#include <string>
#include <iostream>

#include "engine/misc/Debug.h"
using namespace Engine;

Renderer::Renderer(const std::vector<std::string>& shaderFiles, const std::string& shaderProgramName, const std::vector<glm::vec3>& _vertexArray, Material* _material) {
	this->shaderProgram = Engine::ResourceManager::createShaderProgram(shaderFiles, shaderProgramName);
	this->vertexArray = _vertexArray;

	if (_material)
		this->material = _material;
	else
		this->material = new Material();
};

Renderer::~Renderer() { // Does not need to delete shaderProgram as ResourceManager does that
	if (this->VAO != 0) glDeleteVertexArrays(1, &this->VAO);
	if (this->VBO != 0) glDeleteBuffers(1, &this->VBO);
	if (this->EBO != 0) glDeleteBuffers(1, &this->EBO);

	delete this->material;
}

void Renderer::setMaterial(Material* _material) {
	this->material = _material;
}

// TODO: If the indexArray is empty, still calculate the normals by going through vertices 3 by 3
void Renderer::calculateNormals() { 
	if (this->indexArray.size() == 0) {
		LogManager::LogError("Trying to calculate normals while indexArray is empty : currently not supported");
		return;
	}

	this->normalArray = std::vector<glm::vec3>(this->vertexArray.size());

	for (unsigned long long i = 2; i < this->indexArray.size(); i+= 3) {
		glm::vec3 pointA = this->vertexArray[this->indexArray[i - 2]];
		glm::vec3 pointB = this->vertexArray[this->indexArray[i - 1]];
		glm::vec3 pointC = this->vertexArray[this->indexArray[i - 0]];

		glm::vec3 normal = glm::normalize(glm::cross((pointB - pointA), (pointC - pointA)));
		this->normalArray[this->indexArray[i - 2]] += normal;
		this->normalArray[this->indexArray[i - 1]] += normal;
		this->normalArray[this->indexArray[i - 0]] += normal;
	}

	for (int i = 0; i < this->normalArray.size(); i++) {
		this->normalArray[i] = glm::normalize(normalArray[i]);
	}
}

void Renderer::setVertexArray(const std::vector<glm::vec3>& _vertexArray) {
	this->vertexArray = _vertexArray;
}

void Renderer::setNormalArray(const std::vector<glm::vec3>& _normalArray) {
	this->normalArray = _normalArray;
}

void Renderer::setTexCoordArray(const std::vector<glm::vec2>& _texCoordArray) {
	this->texCoordArray = _texCoordArray;
}

void Renderer::setIndexArray(const std::vector<unsigned int>& _indexArray) {
	this->indexArray = _indexArray;
}

void Renderer::addTextureName(const std::string& textureName) {
	this->textureNames.push_back(textureName);
}

int Renderer::getStrideValue() {
	int amountOfFloats = 3; // vertex array is never empty

	if (this->normalArray.size() > 0)
		amountOfFloats += 3;

	if (this->texCoordArray.size() > 0)
		amountOfFloats += 2;

	return amountOfFloats * sizeof(float);
}

void* Renderer::getVertexArrayOffsetPtr() {
	return (void*)0;
}

void* Renderer::getNormalArrayOffsetPtr() {
	return (void*)(3 * sizeof(float));  // vertex array is never empty
}

void* Renderer::getTexCoordArrayOffsetPtr() {
	int amountOfFloats = 3; //  vertex array is never empty

	if (this->normalArray.size() > 0)
		amountOfFloats += 3;

	return (void*)(amountOfFloats * sizeof(float));
}
