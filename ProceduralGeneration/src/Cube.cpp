#include "Cube.h"

#include "engine/ResourceManager.h"
#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "engine/MeshRenderer.h"
#include "engine/ShaderManager.h"

#include "Camera.h"
#include "GameManager.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/ext.hpp"

#include <string>
#include <iostream>

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

Cube::Cube(std::string name) : GameObject(name) {
	// setup components
	this->transform = new Engine::Transform();
	// get shaderProgram for renderer
	Shader* cubeShader = Engine::ResourceManager::createShaderProgram({ "shader.vert", "shader.frag" }, "CubeShader");
	this->renderer = new Engine::MeshRenderer(cubeShader);

	// Setup VAO
	
	glGenVertexArrays(1, &this->renderer->VAO);
	glGenBuffers(1, &this->renderer->VBO);
		
	// Bind Vertex Array Object
	glBindVertexArray(this->renderer->VAO);
		
	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->renderer->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Vertex attributes
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Cube::awake() {

}

void Cube::update(float deltaTime) {
	Camera* camera = dynamic_cast<Camera*>(GameManager::getGamePtr()->findObjectWithName("Camera"));
	glm::vec3 front = glm::normalize(glm::vec3(camera->transform->rotationMatrix[0][2], camera->transform->rotationMatrix[1][2], camera->transform->rotationMatrix[2][2]));
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_Q)) {
		glm::mat4 temp = glm::rotate(glm::mat4(1.0f), deltaTime, front);
		this->transform->rotationMatrix = temp * this->transform->rotationMatrix;
		//this->transform->rotationMatrix = glm::rotate(this->transform->rotationMatrix, deltaTime, front);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_E)) {
		glm::mat4 temp = glm::rotate(glm::mat4(1.0f), -deltaTime, front);
		this->transform->rotationMatrix = temp * this->transform->rotationMatrix;
	}
}

void Cube::fixedUpdate() {

}

void Cube::render() {
	glUseProgram(0);
	// Setup uniforms
	this->renderer->shaderProgram->use();
	this->renderer->shaderProgram->setMat4("model", this->transform->scaleMatrix * this->transform->rotationMatrix * this->transform->positionMatrix);
	this->renderer->shaderProgram->setMat4("view", Camera::viewMatrix);
	this->renderer->shaderProgram->setMat4("projection", Camera::projectionMatrix);
	
	// Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Engine::ResourceManager::getTexture("420x0.jpg")->ID);

	// Use VAO
	glBindVertexArray(this->renderer->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	this->renderer->Render();
}
