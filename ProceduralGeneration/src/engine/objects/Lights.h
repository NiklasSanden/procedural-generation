#pragma once

#include "engine/objects/GameObject.h"
#include "engine/resources/Shader.h"

#include "glm/glm.hpp"

#include <string>

namespace Engine {
	class Light : public Engine::GameObject {
	public:
		Light(const std::string& name) : GameObject(name) { };
	};


	class DirectionalLight : public Light {
	public:
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		DirectionalLight(const std::string& _name, glm::vec3& _direction, const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular) : Light(_name),
			direction(_direction), ambient(_ambient), diffuse(_diffuse), specular(_specular)  {
		}

		void setUniform(Shader* shaderProgram, const glm::mat4& viewMatrix) {
			shaderProgram->setBool("directionalLight.exists", true);
			shaderProgram->setVec3("directionalLight.direction", glm::vec3(viewMatrix * glm::vec4(this->direction, 0.0f)));
			shaderProgram->setVec3("directionalLight.ambient", this->ambient);
			shaderProgram->setVec3("directionalLight.diffuse", this->diffuse);
			shaderProgram->setVec3("directionalLight.specular", this->specular);
		}
	};

	class PointLight : public Light {
	public:
		glm::vec3 position;

		float constant	= 0.0f;
		float linear	= 0.0f;
		float quadratic = 0.0f;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		// Either calculate the attenuation with the distance and functions
		PointLight(const std::string& _name, const glm::vec3& _position, const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, const float distance) : Light(_name),
			position(_position), ambient(_ambient), diffuse(_diffuse), specular(_specular) {
			calculateAttenuation(distance);
		}
		// Or set them manually
		PointLight(const std::string& _name, const glm::vec3& _position, const glm::vec3& _ambient, const glm::vec3& _diffuse, const glm::vec3& _specular, const float _constant, const float _linear, const float _quadratic) : Light(_name),
			position(_position), ambient(_ambient), diffuse(_diffuse), specular(_specular), constant(_constant), linear(_linear), quadratic(_quadratic) {
		}

		void setUniform(Shader* shaderProgram, const glm::mat4& viewMatrix, int index) {
			std::string indexStr = std::to_string(index);
			shaderProgram->setBool("pointLights[" + indexStr + "].exists", true);
			shaderProgram->setVec3("pointLights[" + indexStr + "].position", glm::vec3(viewMatrix * glm::vec4(this->position, 1.0)));

			shaderProgram->setFloat("pointLights[" + indexStr + "].constant", this->constant);
			shaderProgram->setFloat("pointLights[" + indexStr + "].linear", this->linear);
			shaderProgram->setFloat("pointLights[" + indexStr + "].quadratic", this->quadratic);

			shaderProgram->setVec3("pointLights[" + indexStr + "].ambient", this->ambient);
			shaderProgram->setVec3("pointLights[" + indexStr + "].diffuse", this->diffuse);
			shaderProgram->setVec3("pointLights[" + indexStr + "].specular", this->specular);
		}
	private:
		void calculateAttenuation(const float distance) {
			// http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
			// The functions have been found by regression using the data from the tables in the link
			this->constant = 1.0f;
			this->linear = 4.69f * (float)std::pow(distance, -1.01f); // f(x) = 4.69x^(-1.01)
			this->quadratic = 3.64f * this->linear * this->linear + 0.02f * this->linear; // f(x) = 3.64*x^2 + 0.02x
		}
	};

	// TODO: Spotlights
}
