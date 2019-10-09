#pragma once

#include "glm/glm.hpp"

namespace Engine {
	class Material {
	public:
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

		Material() { // Default material
			diffuse = glm::vec3(0.7f, 0.2f, 0.1f);
			specular = glm::vec3(0.3f, 0.3f, 0.3f);
			shininess = 8;
		}
		Material(const glm::vec3& _diffuse, const glm::vec3& _specular, float _shininess)
			: diffuse(_diffuse), specular(_specular), shininess(_shininess) {
		};
	};
}
