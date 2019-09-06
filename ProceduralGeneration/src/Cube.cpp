#include "Cube.h"

#include "engine/ResourceManager.h"
#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "engine/MeshRenderer.h"
#include "engine/ShaderManager.h"
#include "engine/Game.h"
#include "engine/Shader.h"
#include "engine/Texture2D.h"
#include "engine/MeshRenderer.h"

#include "Camera.h"
#include "GameManager.h"
#include "Input.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <string>
#include <iostream>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

namespace ProceduralGeneration {
	float vertices[] = {
	// position (vec3) - texture coordinate (vec2)
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
}

Cube::Cube(std::string& name) : GameObject(name) {
	// setup components
	this->transform = new Engine::Transform();

	// get shaderProgram for renderer
	std::vector<std::string> shaderFiles = { "shader.vert", "shader.frag" };
	std::string shaderProgramName = "CubeShader";
	Engine::Shader* cubeShader = Engine::ResourceManager::createShaderProgram(shaderFiles, shaderProgramName);
	this->renderer = new Engine::MeshRenderer(cubeShader);

	// Setup VAO
	glGenVertexArrays(1, &this->renderer->VAO);
	glGenBuffers(1, &this->renderer->VBO);
		
	glBindVertexArray(this->renderer->VAO);
		
	glBindBuffer(GL_ARRAY_BUFFER, this->renderer->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::awake() {

}

void Cube::update(float deltaTime) {
	std::string cameraName = "Camera";
	Camera* camera = dynamic_cast<Camera*>(GameManager::getGamePtr()->findObjectWithName(cameraName));
	glm::vec3 front = -camera->transform->getDirection();
	
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_Q)) {
		this->transform->rotate(deltaTime, front);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_E)) {
		this->transform->rotate(-deltaTime, front);
	}

	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_L)) {
		this->transform->translate(this->transform->getRight() * deltaTime);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_J)) {
		this->transform->translate(-this->transform->getRight() * deltaTime);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_I)) {
		this->transform->translate(this->transform->getUp() * deltaTime);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_K)) {
		this->transform->translate(-this->transform->getUp() * deltaTime);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_U)) {
		this->transform->translate(this->transform->getDirection() * deltaTime);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_O)) {
		this->transform->translate(-this->transform->getDirection() * deltaTime);
	}
}

void Cube::fixedUpdate() {

}

void Cube::render() {
	// Setup uniforms
	this->renderer->shaderProgram->use();
	this->renderer->shaderProgram->setMat4("model", this->transform->getModelMatrix());
	this->renderer->shaderProgram->setMat4("view", Camera::viewMatrix);
	this->renderer->shaderProgram->setMat4("projection", Camera::projectionMatrix);
	
	// Textures
	glActiveTexture(GL_TEXTURE0);
	std::string textureName = "fire.jpg";
	glBindTexture(GL_TEXTURE_2D, Engine::ResourceManager::getTexture(textureName)->ID);

	// Use VAO
	glBindVertexArray(this->renderer->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Unbind
	glBindVertexArray(0);
	this->renderer->shaderProgram->unbind();

	// Renderer component
	this->renderer->Render();
}
