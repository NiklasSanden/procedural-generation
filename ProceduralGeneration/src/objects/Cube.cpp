#include "objects/Cube.h"

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

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/norm.hpp"

#include <string>
#include <iostream>
#include <map>

#include "engine/misc/Debug.h"
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

Cube::Cube(const std::string& name) : GameObject(name) {
	// setup components
	this->transform = new Engine::Transform();

	// get shaderProgram for renderer
	this->renderer = new Engine::MeshRenderer({ "default.vert", "default.frag" }, "CubeShader", vertexArray);
	
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

void Cube::update(float deltaTime) {
	Camera* camera = dynamic_cast<Camera*>(GameManager::getGamePtr()->findObjectWithName("Camera"));
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
