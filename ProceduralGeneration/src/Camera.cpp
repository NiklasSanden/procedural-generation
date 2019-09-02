#include "Camera.h"

#include "GameManager.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ProceduralGeneration;

void Camera::awake() {

}

void Camera::update(float deltaTime) {
	// Looking around
	glm::vec2 mouseDelta = GameManager::getInputPtr()->getMouseDelta();
}

void Camera::fixedUpdate() {

}

// Rendering for the camera object will be used to render the scene :)
void Camera::render() {

}
