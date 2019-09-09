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

	generateChunk();
}

Chunk::~Chunk() {

}

void Chunk::update(float deltaTime) {

}

void Chunk::fixedUpdate() {

}

void Chunk::render() {
	if (!this->doneGeneratingChunk) return;

	// Check to see if possible that the chunk might be seen by the camera
	glm::vec3 margin = -GameManager::getPlayer()->transform->getDirection() * this->chunkManager->chunkDistaceToCorner;
	glm::vec3 difference = this->transform->getPosition() - GameManager::getPlayer()->transform->getPosition() + margin;
	if (glm::dot(difference, -GameManager::getPlayer()->transform->getDirection()) <= 0.0f) {
		return;
	}

	// used in multiple function calls
	glm::mat4 modelMatrix = this->transform->getModelMatrix();

	this->renderer->shaderProgram->use();
	// directional light
	if (GameManager::getDirectionalLight()) {
		GameManager::getDirectionalLight()->setUniform(this->renderer->shaderProgram, Camera::viewMatrix);
	}
	else { // let the shader know that a directional light doesn't exist
		this->renderer->shaderProgram->setBool("directionalLight.exists", false);
	}

	// render the four closest point lights
	std::multimap<float, Engine::PointLight*> pointLightsOrder;
	for (int i = 0; i < GameManager::getPointLights().size(); i++) {
		float distanceSqrd = glm::length2(GameManager::getPointLights()[i]->position - this->transform->getPosition());
		pointLightsOrder.insert(std::make_pair(distanceSqrd, GameManager::getPointLights()[i]));
	}

	int counter = 0;
	for (auto& it = pointLightsOrder.begin(); it != pointLightsOrder.end() && counter < 4; it++, counter++) {
		it->second->setUniform(this->renderer->shaderProgram, Camera::viewMatrix, counter);
	}

	// we have to tell the shader that the rest don't exist
	for (; counter < 4; counter++) {
		this->renderer->shaderProgram->setBool("pointLights[" + std::to_string(counter) + "].exists", false);
	}

	this->renderer->render(modelMatrix, Camera::viewMatrix, Camera::projectionMatrix, this->transform->getNormalMatrix(modelMatrix, Camera::viewMatrix));
}

void Chunk::renderImGui() {

}

void Chunk::generateChunk() {
	// renderer
	std::vector<glm::vec3> vertexArray{
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

	std::vector<unsigned int> indexArray{ 0, 2, 1, 0, 3, 2,
									  4, 5, 6, 4, 6, 7,
									  8, 9, 10, 8, 10, 11,
									  12, 14, 13, 12, 15, 14,
									  16, 17, 18, 16, 18, 19,
									  20, 22, 21, 20, 23, 22, };


	this->renderer = new Engine::MeshRenderer({ "chunk.vert", "chunk.frag" }, "ChunkShader", vertexArray);

	this->renderer->setIndexArray(indexArray);

	this->renderer->calculateNormals();

	this->renderer->setupVertexArrayObject();

	this->doneGeneratingChunk = true;
}
