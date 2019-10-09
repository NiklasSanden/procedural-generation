#pragma once

#include "glm/glm.hpp"

namespace Engine {
	class Transform {
	protected:
		glm::mat4 positionMatrix = glm::mat4(1.0f);
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		glm::mat4 scaleMatrix = glm::mat4(1.0f);
	public:
		Transform() {};
		~Transform() {};

		glm::mat4 getModelMatrix();
		glm::mat3 getNormalMatrix(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix);

		glm::vec3 getPosition();
		glm::quat getRotation();
		glm::vec3 getScale();

		glm::vec3 getRight();
		glm::vec3 getUp();
		glm::vec3 getDirection();

		
		void setPosition(const glm::vec3& position);
		void setRotation(const glm::quat& rotation);
		void setScale(const glm::vec3& scale);

		void setRight(const glm::vec3& right);
		void setUp(const glm::vec3& up);
		void setDirection(const glm::vec3& front);

		
		void translate(const glm::vec3& position);
		void rotate(float angle, const glm::vec3& axis);
		void scale(const glm::vec3& scale);

		void normalizeRotation();
	};
}
