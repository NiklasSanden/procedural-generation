#include "setup/Camera.h"

#include "engine/misc/Program.h"
#include "engine/objects/GameObject.h"
#include "engine/components/Transform.h"
#include "setup/GameManager.h"
#include "setup/Input.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/vector_angle.hpp"

#include <iostream>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

// Declare static variables
glm::mat4 Camera::viewMatrix = glm::mat4(1.0f);
glm::mat4 Camera::projectionMatrix = glm::mat4(1.0f);
float Camera::verticalFov = 60.0f;
float Camera::viewDistance = 25.0f;

Camera::Camera(const std::string& name) : GameObject(name) {
	this->transform = new Engine::Transform();
	this->transform->translate(glm::vec3(0.0f, 0.0f, 20.0f));
}

void Camera::update(float deltaTime) {
	// Looking around
	glm::vec2 mouseDelta = GameManager::getInputPtr()->getMouseDelta();
	// Make sure your neck is intact
	if (anglesRotatedX + mouseDelta.y > glm::pi<float>() / 2.0f - 0.1f && mouseDelta.y > 0.0f) {
		mouseDelta.y = (glm::pi<float>() / 2.0f - 0.1f) - anglesRotatedX;
	}
	else if (anglesRotatedX + mouseDelta.y < -glm::pi<float>() / 2.0f + 0.1f && mouseDelta.y < 0.0f) {
		mouseDelta.y = (-glm::pi<float>() / 2.0f + 0.1f) - anglesRotatedX;
	}
	
	this->transform->rotate(mouseDelta.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 rightXZ = glm::normalize(glm::vec3(this->transform->getRight().x, 0.0f, this->transform->getRight().z));
	this->transform->rotate(-mouseDelta.y, rightXZ);

	// now that we've looked around, get the front and right vector
	glm::vec3 front = -this->transform->getDirection();
	glm::vec3 frontXZ = glm::normalize(glm::vec3(front.x, 0.0f, front.z));

	anglesRotatedX = glm::angle(glm::normalize(front), glm::normalize(glm::vec3(front.x, 0.0, front.z)));
	if (front.y < 0.0f) 
		anglesRotatedX *= -1;

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
	// move
	if (glm::abs(movement.x) > 0.1f || glm::abs(movement.y) > 0.1f) {
		movement = glm::normalize(movement);
		this->transform->translate(frontXZ * movement.y * this->movementSpeed * deltaTime);
		this->transform->translate(rightXZ * movement.x * this->movementSpeed * deltaTime);
	}

	// up and down
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
	projectionMatrix = glm::perspective(glm::radians(verticalFov), (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT, 0.1f, 500.0f);

	// view matrix
	viewMatrix = glm::lookAt(this->transform->getPosition(), this->transform->getPosition() - this->transform->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::fixedUpdate() {

}

void Camera::render() {

}
