#pragma once

#include "glm/glm.hpp"

namespace Engine {
	class Material {
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

		Material() { // Default material
			ambient = glm::vec3(0.7f, 0.2f, 0.1f);
			diffuse = glm::vec3(0.7f, 0.2f, 0.1f);
			specular = glm::vec3(0.5f, 0.5f, 0.5f);
			shininess = 32;
		}
		Material(glm::vec3& _ambient, glm::vec3 _diffuse, glm::vec3& _specular, float _shininess)
			: ambient(_ambient), diffuse(_diffuse), specular(_specular), shininess(_shininess) {
		};
	};
}
