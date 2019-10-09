#include "Chunk.h"

#include "engine/resources/ResourceManager.h"
#include "engine/objects/GameObject.h"
#include "engine/components/Transform.h"
#include "engine/components/MeshRenderer.h"
#include "engine/resources/ShaderManager.h"
#include "engine/misc/Game.h"
#include "engine/resources/Shader.h"
#include "engine/resources/Texture2D.h"
#include "engine/components/MeshRenderer.h"
#include "engine/objects/Lights.h"

#include "setup/Camera.h"
#include "setup/GameManager.h"
#include "setup/Input.h"
#include "objects/ChunkManager.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/norm.hpp"

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

Chunk::Chunk(const std::string& name, const glm::vec3& position, ChunkManager* _chunkManager) : GameObject(name) {
	this->chunkManager = _chunkManager;
	
	// setup components
	this->transform = new Engine::Transform();
	this->transform->translate(position);
	this->transform->scale(glm::vec3(this->chunkManager->chunkLength / 1.2f));

	this->renderer = new Engine::Renderer({ "chunk.vert", "chunk.frag" }, "ChunkShader", {});

	generateChunk();
}

Chunk::~Chunk() {

}

void Chunk::update(float deltaTime) {

}

void Chunk::fixedUpdate() {

}

void Chunk::generateChunk() {
	// TODO: Generation Algorithm

	// renderer
	std::vector<glm::vec3> vertexArray = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),

		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),

		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),

		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),

		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),

		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
	};
	for (int i = 0; i < vertexArray.size(); i++) {
		vertexArray[i] = glm::vec3(this->transform->getModelMatrix() * glm::vec4(vertexArray[i], 1.0f));
	}

	std::vector<unsigned int> indexArray = { 0, 2, 1, 0, 3, 2,
						 4, 5, 6, 4, 6, 7,
						 8, 9, 10, 8, 10, 11,
						 12, 14, 13, 12, 15, 14,
						 16, 17, 18, 16, 18, 19,
						 20, 22, 21, 20, 23, 22, };

	this->renderer->setVertexArray(vertexArray);
	this->renderer->setIndexArray(indexArray);

	this->renderer->calculateNormals();


	if (this->renderer->vertexArray.size() != this->renderer->normalArray.size()) {
		Engine::LogManager::LogError("VertexArray doesn't have the same size as normalArray in ChunkManager");
		return;
	}
	// Precalculate vertexAttributes
	this->vertexAttributes.reserve(this->renderer->vertexArray.size() * 3 + this->renderer->normalArray.size() * 3);
	for (unsigned int i = 0; i < this->renderer->vertexArray.size(); i++) {
		this->vertexAttributes.push_back(this->renderer->vertexArray[i].x);
		this->vertexAttributes.push_back(this->renderer->vertexArray[i].y);
		this->vertexAttributes.push_back(this->renderer->vertexArray[i].z);

		this->vertexAttributes.push_back(this->renderer->normalArray[i].x);
		this->vertexAttributes.push_back(this->renderer->normalArray[i].y);
		this->vertexAttributes.push_back(this->renderer->normalArray[i].z);
	}

	this->doneGeneratingChunk = true;
}

void Chunk::getArraysForRendering(std::vector<float>*& _vertexAttributes, std::vector<unsigned int>*& _indexArray) {
	if (!this->doneGeneratingChunk) {
		_vertexAttributes = nullptr;
		_indexArray = nullptr;
		return;
	}

	// Check to see if possible that the chunk might be seen by the camera
	glm::vec3 margin = -GameManager::getPlayer()->transform->getDirection() * this->chunkManager->chunkDistaceToCorner;
	glm::vec3 difference = this->transform->getPosition() - GameManager::getPlayer()->transform->getPosition() + margin;
	if (glm::dot(difference, -GameManager::getPlayer()->transform->getDirection()) <= 0.0f) {
		_vertexAttributes = nullptr;
		_indexArray = nullptr;
		return;
	}
	
	_vertexAttributes = &this->vertexAttributes;
	_indexArray = &this->renderer->indexArray;
}
