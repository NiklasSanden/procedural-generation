#include "MeshRenderer.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Material.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <vector>
#include <string>
#include <iostream>

#include "Debug.h"
using namespace Engine;

MeshRenderer::MeshRenderer(std::vector<std::string>& shaderFiles, std::string& shaderProgramName, std::vector<glm::vec3>& _vertexArray, Material* _material) 
	: Renderer(shaderFiles, shaderProgramName, _vertexArray, _material) {

}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::setupVertexArrayObject() {
	// The vertexArray cannot be empty since it is needed in vertex shaders
	if (this->vertexArray.size() == 0) {
		std::cout << "---------> Error - vertexArray empty" << std::endl;
		return;
	}
	// All of the arrays have to be the same size, excluding empty ones
	if ((this->vertexArray.size() != this->normalArray.size()	&& this->normalArray.size()	  != 0) ||
		(this->vertexArray.size() != this->texCoordArray.size() && this->texCoordArray.size() != 0)) {
		std::cout << "---------> Error - The vertexArray, normalArray, and texCoordArray are not the same size (excluding empty ones)" << std::endl;
		return;
	}
	
	// Combine vector data
	std::vector<float> vertexAttributes(this->vertexArray.size() * 3 + this->normalArray.size() * 3 + this->texCoordArray.size() * 2); // the multiplications comes from vec3 being 3 floats etc.

	long long vertexAttributesIndex = 0; // Visual studio gives weird arithmetic overflow warning when this is an int, because of the addition and then the cast to 8 bytes
	for (int i = 0; i < this->vertexArray.size(); i++) {
		vertexAttributes[vertexAttributesIndex + 0] = this->vertexArray[i].x;
		vertexAttributes[vertexAttributesIndex + 1] = this->vertexArray[i].y;
		vertexAttributes[vertexAttributesIndex + 2] = this->vertexArray[i].z;
		vertexAttributesIndex += 3;

		if (this->normalArray.size() > 0) {
			vertexAttributes[vertexAttributesIndex + 0] = this->normalArray[i].x;
			vertexAttributes[vertexAttributesIndex + 1] = this->normalArray[i].y;
			vertexAttributes[vertexAttributesIndex + 2] = this->normalArray[i].z;
			vertexAttributesIndex += 3;
		}

		if (this->texCoordArray.size() > 0) {
			vertexAttributes[vertexAttributesIndex + 0] = this->texCoordArray[i].x;
			vertexAttributes[vertexAttributesIndex + 1] = this->texCoordArray[i].y;
			vertexAttributesIndex += 2;
		}
	}

	// Setup VAO
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexAttributes.size() * sizeof(float), vertexAttributes.data(), GL_STATIC_DRAW);


	// position attribute
	int vertexAttributeCounter = 0;
	glVertexAttribPointer(vertexAttributeCounter, 3, GL_FLOAT, GL_FALSE, getStrideValue(), getVertexArrayOffsetPtr());
	glEnableVertexAttribArray(vertexAttributeCounter);
	vertexAttributeCounter++;

	// normals attribute
	if (this->normalArray.size() > 0) {
		glVertexAttribPointer(vertexAttributeCounter, 3, GL_FLOAT, GL_FALSE, getStrideValue(), getNormalArrayOffsetPtr());
		glEnableVertexAttribArray(vertexAttributeCounter);
		vertexAttributeCounter++;
	}

	// texture coordinates attribute
	if (this->texCoordArray.size() > 0) {
		glVertexAttribPointer(vertexAttributeCounter, 2, GL_FLOAT, GL_FALSE, getStrideValue(), getTexCoordArrayOffsetPtr());
		glEnableVertexAttribArray(vertexAttributeCounter);
		vertexAttributeCounter++;
	}

	// index array
	if (this->indexArray.size() > 0) {
		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size() * sizeof(unsigned int), this->indexArray.data(), GL_STATIC_DRAW);
		// We cannot unbind it here, since the vertexArrayObject keeps track of the binded EBO at the end, and would therefore not use this
		// Unbinding happens at the end of the function
	}

	// unbind all
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (this->indexArray.size() > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::render(glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat3& normalMatrix) {
	if (this->VAO == 0) {
		std::cout << "---------> Error - Trying to render (MeshRenderer) without first calling setupVertexArrayObject()" << std::endl;
		return;
	}
	if (this->vertexArray.size() == 0) return; // We might want to display a message, but that could clog up the console if we are working on an object that calls render without wanting to render

	// Uniforms
	this->shaderProgram->use();
	this->shaderProgram->setMat4("model", modelMatrix);
	this->shaderProgram->setMat4("view", viewMatrix);
	this->shaderProgram->setMat4("projection", projectionMatrix);
	this->shaderProgram->setMat3("normal", normalMatrix);
	// material
	this->shaderProgram->setVec3("material.ambient",	this->material->ambient);
	this->shaderProgram->setVec3("material.diffuse",	this->material->diffuse);
	this->shaderProgram->setVec3("material.specular",	this->material->specular);
	this->shaderProgram->setFloat("material.shininess", this->material->shininess);

	// Textures
	for (int i = 0; i < this->textureNames.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Engine::ResourceManager::getTexture(textureNames[i])->ID);
	}

	// Use VAO
	glBindVertexArray(this->VAO);
	if (this->indexArray.size() > 0) { // If we have indices, then we want to use them to draw
		glDrawElements(GL_TRIANGLES, (GLsizei)this->indexArray.size(), GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->vertexArray.size());
	}

	// Unbind
	glBindVertexArray(0);
	this->shaderProgram->unbind();
}
