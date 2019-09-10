#include "Chunk.h"

#include "engine/ResourceManager.h"
#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "engine/MeshRenderer.h"
#include "engine/ShaderManager.h"
#include "engine/Game.h"
#include "engine/Shader.h"
#include "engine/Texture2D.h"
#include "engine/MeshRenderer.h"
#include "engine/Lights.h"

#include "Camera.h"
#include "GameManager.h"
#include "Input.h"
#include "ChunkManager.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/norm.hpp"

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "engine/Debug.h"
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

	this->doneGeneratingChunk = true;
}

void Chunk::getArraysForRendering(std::vector<glm::vec3>*& _vertexArray, std::vector<unsigned int>*& _indexArray, std::vector<glm::vec3>*& _normalArray) {
	if (!this->doneGeneratingChunk) {
		_vertexArray = nullptr;
		_indexArray = nullptr;
		_normalArray = nullptr;
		return;
	}

	// Check to see if possible that the chunk might be seen by the camera
	glm::vec3 margin = -GameManager::getPlayer()->transform->getDirection() * this->chunkManager->chunkDistaceToCorner;
	glm::vec3 difference = this->transform->getPosition() - GameManager::getPlayer()->transform->getPosition() + margin;
	if (glm::dot(difference, -GameManager::getPlayer()->transform->getDirection()) <= 0.0f) {
		_vertexArray = nullptr;
		_indexArray = nullptr;
		_normalArray = nullptr;
		return;
	}
	
	_vertexArray = &this->renderer->vertexArray;
	_indexArray = &this->renderer->indexArray;
	_normalArray = &this->renderer->normalArray;
}
