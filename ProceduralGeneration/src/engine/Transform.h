#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine {
	class Transform {
	protected:
		glm::mat4 positionMatrix = glm::mat4(1.0f);
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);
	public:
		Transform() {};
		~Transform() {};

		// Get the model matrix
		glm::mat4 getModelMatrix();

		// Extract from matrices
		glm::vec3 getPosition();
		glm::quat getRotation();
		glm::vec3 getScale();

		glm::vec3 getRight();
		glm::vec3 getUp();
		glm::vec3 getDirection();

		// Set values in matrices
		void setPosition(glm::vec3 position);
		void setRotation(glm::quat rotation);
		void setScale(glm::vec3 scale);

		void setRight(glm::vec3 right);
		void setUp(glm::vec3 up);
		void setDirection(glm::vec3 front);

		// Add to existing matrices
		void translate(glm::vec3 position);
		void rotate(float angle, glm::vec3 axis);
		void scale(glm::vec3 scale);

		// Normalize rotationMatrix
		void normalizeRotation();
	};
}
