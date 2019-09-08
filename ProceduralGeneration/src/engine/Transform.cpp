#include "Transform.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "Debug.h"
using namespace Engine;

glm::mat4 Transform::getModelMatrix() {
	// We must use the transpose of the rotationMatrix (https://en.wikipedia.org/wiki/Rotation_matrix)
	// "If any one of these is changed (such as rotating axes instead of vectors, a passive transformation), then the inverse of the example matrix should be used, which coincides with its transpose."
	// We are rotating axes
	return this->positionMatrix * glm::transpose(rotationMatrix) * this->scaleMatrix;
}

glm::mat3 Transform::getNormalMatrix(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix) {
	// What this does can be read here: https://learnopengl.com/Lighting/Basic-Lighting
	return glm::mat3(glm::transpose(glm::inverse(viewMatrix * modelMatrix)));
}

glm::vec3 Transform::getPosition() {
	return glm::vec3(this->positionMatrix[3][0], this->positionMatrix[3][1], this->positionMatrix[3][2]);
}

glm::quat Transform::getRotation() {
	return glm::quat(this->rotationMatrix);
}

glm::vec3 Transform::getScale() {
	return glm::vec3(this->scaleMatrix[0][0], this->scaleMatrix[1][1], this->scaleMatrix[2][2]);
}

glm::vec3 Transform::getRight() {
	return glm::vec3(this->rotationMatrix[0][0], this->rotationMatrix[1][0], this->rotationMatrix[2][0]);
}

glm::vec3 Transform::getUp() {
	return glm::vec3(this->rotationMatrix[0][1], this->rotationMatrix[1][1], this->rotationMatrix[2][1]);
}

glm::vec3 Transform::getDirection() {
	return glm::vec3(this->rotationMatrix[0][2], this->rotationMatrix[1][2], this->rotationMatrix[2][2]);
}

void Transform::setPosition(const glm::vec3& position) {
	this->positionMatrix[3][0] = position.x;
	this->positionMatrix[3][1] = position.y;
	this->positionMatrix[3][2] = position.z;
}

void Transform::setRotation(const glm::quat& rotation) {
	this->rotationMatrix = glm::mat4_cast(rotation);
}

void Transform::setScale(const glm::vec3& scale) {
	this->scaleMatrix[0][0] = scale.x;
	this->scaleMatrix[1][1] = scale.y;
	this->scaleMatrix[2][2] = scale.z;
}

void Transform::setRight(const glm::vec3& right) {
	this->rotationMatrix[0][0] = right.x;
	this->rotationMatrix[1][0] = right.y;
	this->rotationMatrix[2][0] = right.z;
}
void Transform::setUp(const glm::vec3& up) {
	this->rotationMatrix[0][1] = up.x;
	this->rotationMatrix[1][1] = up.y;
	this->rotationMatrix[2][1] = up.z;
}
void Transform::setDirection(const glm::vec3& direction) {
	this->rotationMatrix[0][2] = direction.x;
	this->rotationMatrix[1][2] = direction.y;
	this->rotationMatrix[2][2] = direction.z;
}

void Transform::translate(const glm::vec3& position) {
	this->positionMatrix[3][0] += position.x;
	this->positionMatrix[3][1] += position.y;
	this->positionMatrix[3][2] += position.z;
}

void Transform::rotate(float angle, const glm::vec3& axis) {
	// Create a rotation matrix based on the desired rotations
	glm::quat tempRot = glm::angleAxis(angle, axis);
	setRotation(getRotation() * tempRot); // not commutative

	// Make sure that the right, up and direction vectors are normalized (will change after a while because of floating point precision error)
	normalizeRotation();
}

void Transform::scale(const glm::vec3& scale) {
	this->scaleMatrix[0][0] *= scale.x;
	this->scaleMatrix[1][1] *= scale.y;
	this->scaleMatrix[2][2] *= scale.z;
}

void Transform::normalizeRotation() {
	setRight(glm::normalize(getRight()));
	setUp(glm::normalize(getUp()));
	setDirection(glm::normalize(getDirection()));
}
