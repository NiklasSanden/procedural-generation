#include "Camera.h"

#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "GameManager.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

using namespace ProceduralGeneration;

Camera::Camera() : GameObject("Camera") {
	this->transform = new Engine::Transform();
}

void Camera::awake() {

}

void Camera::update(float deltaTime) {
	// Looking around
	glm::vec2 mouseDelta = GameManager::getInputPtr()->getMouseDelta();
	this->transform->rotationMatrix = glm::rotate(this->transform->rotationMatrix, mouseDelta.x, glm::vec3(0.0f, 1.0f, 0.0f));
	std::cout << mouseDelta.x << std::endl;
}

void Camera::fixedUpdate() {

}

// Rendering for the camera object will be used to render the scene :)
void Camera::render() {

}
