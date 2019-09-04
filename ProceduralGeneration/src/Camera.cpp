#include "Camera.h"

#include "engine/Program.h"
#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "GameManager.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include <iostream>

using namespace ProceduralGeneration;

// Declare static variables
glm::mat4 Camera::viewMatrix = glm::mat4(1.0f);
glm::mat4 Camera::projectionMatrix = glm::mat4(1.0f);

Camera::Camera() : GameObject("Camera") {
	this->transform = new Engine::Transform();

	// move back
	this->transform->translate(glm::vec3(0.0f, 0.0f, 5.0f));
	//this->transform->positionMatrix = glm::translate(this->transform->positionMatrix, glm::vec3(0.0f, 0.0f, 5.0f));
}

void Camera::awake() {

}

void Camera::update(float deltaTime) {
	// Looking around
	glm::vec2 mouseDelta = GameManager::getInputPtr()->getMouseDelta();
	// Make sure your neck is intact
	if (anglesRotatedX + mouseDelta.y > glm::pi<float>() / 2.0f - 0.1f) {
		mouseDelta.y = (glm::pi<float>() / 2.0f - 0.1f) - anglesRotatedX;
	}
	else if (anglesRotatedX + mouseDelta.y < -glm::pi<float>() / 2.0f + 0.1f) {
		mouseDelta.y = (-glm::pi<float>() / 2.0f + 0.1f) - anglesRotatedX;
	}
	anglesRotatedX += mouseDelta.y;
	
	// rotate around y-axis
	this->transform->rotate(mouseDelta.x, glm::vec3(0.0f, 1.0f, 0.0f));
	// rotate around right-axis
	this->transform->rotate(mouseDelta.y, this->transform->getRight());

	// now that we've looked around, get the front and right vector
	glm::vec3 front = -this->transform->getDirection();
	glm::vec3 right = this->transform->getRight();
	glm::vec3 frontXZ = glm::normalize(glm::vec3(front.x, 0.0f, front.z));

	// Movement
	glm::vec2 movement(0.0f, 0.0f);
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_W)) {
		movement.y += 1.0f;
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_S)) {
		movement.y -= 1.0f;
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_D)) {
		movement.x += 1.0f;
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_A)) {
		movement.x -= 1.0f;
	}
	if (glm::abs(movement.x) > 0.1f || glm::abs(movement.y) > 0.1f) {
		// Move
		movement = glm::normalize(movement);
		this->transform->translate(frontXZ * movement.y * this->movementSpeed * deltaTime);
		this->transform->translate(right * movement.x * this->movementSpeed * deltaTime);
	}

	// Up and down
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		this->transform->translate(glm::vec3(0.0f, -this->movementSpeed, 0.0f) * deltaTime);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_SPACE)) {
		this->transform->translate(glm::vec3(0.0f, this->movementSpeed, 0.0f) * deltaTime);
	}
	
	
	// -----------------------------
	// Update Matrices
	// -----------------------------
	// projection matrix
	projectionMatrix = glm::perspective(glm::radians(60.0f), (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT, 0.1f, 100.0f);

	// view matrix
	viewMatrix = glm::lookAt(this->transform->getPosition(), this->transform->getPosition() - this->transform->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::fixedUpdate() {

}

void Camera::render() {

}
