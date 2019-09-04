#include "Camera.h"

#include "engine/Program.h"
#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "GameManager.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/ext.hpp"

#include <iostream>

using namespace ProceduralGeneration;

// Declare static variables
glm::mat4 Camera::viewMatrix = glm::mat4(1.0f);
glm::mat4 Camera::projectionMatrix = glm::mat4(1.0f);

Camera::Camera() : GameObject("Camera") {
	this->transform = new Engine::Transform();

	// move back
	this->transform->positionMatrix = glm::translate(this->transform->positionMatrix, glm::vec3(0.0f, 0.0f, 5.0f));
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
	
	glm::mat4 tempRotationMatrix(1.0f);
	this->transform->rotationMatrix = this->transform->rotationMatrix * glm::rotate(tempRotationMatrix, mouseDelta.x, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 tempRight = glm::vec3(this->transform->rotationMatrix[0][0], this->transform->rotationMatrix[1][0], this->transform->rotationMatrix[2][0]);
	tempRotationMatrix = glm::mat4(1.0f);
	this->transform->rotationMatrix = this->transform->rotationMatrix * glm::rotate(tempRotationMatrix, mouseDelta.y, tempRight);

	//std::cout << glm::length(glm::vec3(this->transform->rotationMatrix[0][0], this->transform->rotationMatrix[1][0], this->transform->rotationMatrix[2][0])) <<
	//	" - " << glm::length(glm::vec3(this->transform->rotationMatrix[0][1], this->transform->rotationMatrix[1][1], this->transform->rotationMatrix[2][1])) <<
	//	" - " << glm::length(glm::vec3(this->transform->rotationMatrix[0][2], this->transform->rotationMatrix[1][2], this->transform->rotationMatrix[2][2])) << std::endl;
	
	std::cout << this->transform->rotationMatrix[0][0] << ", " << this->transform->rotationMatrix[1][0] << ", " << this->transform->rotationMatrix[2][0] <<
		" - " << this->transform->rotationMatrix[0][1] << ", " << this->transform->rotationMatrix[1][1] << ", " << this->transform->rotationMatrix[2][1] <<
		" - " << this->transform->rotationMatrix[0][2] << ", " << this->transform->rotationMatrix[1][2] << ", " << this->transform->rotationMatrix[2][2] << std::endl;

	// now that we've looked around, get the front and right vector
	glm::vec3 front = -glm::vec3(this->transform->rotationMatrix[0][2], this->transform->rotationMatrix[1][2], this->transform->rotationMatrix[2][2]);
	glm::vec3 right = glm::vec3(this->transform->rotationMatrix[0][0], this->transform->rotationMatrix[1][0], this->transform->rotationMatrix[2][0]);
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
		this->transform->positionMatrix = glm::translate(this->transform->positionMatrix, frontXZ * movement.y * this->movementSpeed * deltaTime);
		this->transform->positionMatrix = glm::translate(this->transform->positionMatrix, right * movement.x * this->movementSpeed * deltaTime);
	}

	// Up and down
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		this->transform->positionMatrix = glm::translate(this->transform->positionMatrix, glm::vec3(0.0f, -this->movementSpeed, 0.0f) * deltaTime);
	}
	if (GameManager::getInputPtr()->isKeyPressed(GLFW_KEY_SPACE)) {
		this->transform->positionMatrix = glm::translate(this->transform->positionMatrix, glm::vec3(0.0f, this->movementSpeed, 0.0f) * deltaTime);
	}
	
	
	//std::cout << 1.0f / deltaTime << " - Position: (" << this->transform->positionMatrix[3][0] << ", " << this->transform->positionMatrix[3][1] << ", " << this->transform->positionMatrix[3][2] << ")" << std::endl;


	// -----------------------------
	// Update Matrices
	// -----------------------------
	// projection matrix
	projectionMatrix = glm::perspective(glm::radians(45.0f), (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT, 0.1f, 100.0f);

	// view matrix
	glm::vec3 position(this->transform->positionMatrix[3][0], this->transform->positionMatrix[3][1], this->transform->positionMatrix[3][2]);

	viewMatrix = glm::lookAt(position, position + front, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::fixedUpdate() {

}

void Camera::render() {

}
