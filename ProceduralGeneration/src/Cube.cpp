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
	std::vector<glm::vec2> texCoordArray{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
	};
}

Cube::Cube(std::string& name) : GameObject(name) {
	// setup components
	this->transform = new Engine::Transform();

	// get shaderProgram for renderer
	std::vector<std::string> shaderFiles = { "default.vert", "default.frag" };
	std::string shaderProgramName = "CubeShader";
	this->renderer = new Engine::MeshRenderer(shaderFiles, shaderProgramName, vertexArray);
	
	// texture
	/*std::string textureName = "fire.jpg";
	this->renderer->addTextureName(textureName);
	this->renderer->setTexCoordArray(texCoordArray);*/

	std::vector<unsigned int> indexArray{ 0, 2, 1, 0, 3, 2,
										  4, 5, 6, 4, 6, 7, 
										  8, 9, 10, 8, 10, 11, 
										  12, 14, 13, 12, 15, 14,
										  16, 17, 18, 16, 18, 19, 
										  20, 22, 21, 20, 23, 22, };
	this->renderer->setIndexArray(indexArray);
	
	this->renderer->calculateNormals();

	this->renderer->setupVertexArrayObject();
}

Cube::~Cube() {

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
	this->renderer->shaderProgram->use();
	this->renderer->shaderProgram->setVec3("light.position", glm::vec3(2.0f, 2.0f, 2.0f));
	this->renderer->shaderProgram->setVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
	this->renderer->shaderProgram->setVec3("light.diffuse",  glm::vec3(1.0f, 1.0f, 1.0f));
	this->renderer->shaderProgram->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	glm::mat4 modelMatrix = this->transform->getModelMatrix();
	this->renderer->render(modelMatrix, Camera::viewMatrix, Camera::projectionMatrix, this->transform->getNormalMatrix(modelMatrix, Camera::viewMatrix));
}
