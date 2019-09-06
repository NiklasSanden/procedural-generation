#pragma once

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace Engine {
	class Shader
	{
	public:
		unsigned int ID;
		// cache uniform positions
		std::unordered_map<std::string, int> uniformLocations;

		Shader(const std::vector<unsigned int>& shaderIDs);
		~Shader();

		void use();
		void unbind();

		void setBool(const std::string& name, bool value);
		void setInt(const std::string& name, int value);
		void setFloat(const std::string& name, float value);

		void setVec2(const std::string& name, const glm::vec2& value);
		void setVec2(const std::string& name, float x, float y);

		void setVec3(const std::string& name, const glm::vec3& value);
		void setVec3(const std::string& name, float x, float y, float z);

		void setVec4(const std::string& name, const glm::vec4& value);
		void setVec4(const std::string& name, float x, float y, float z, float w);

		void setMat2(const std::string& name, const glm::mat2& value);
		void setMat3(const std::string& name, const glm::mat3& value);
		void setMat4(const std::string& name, const glm::mat4& value);


	private:
		void checkLinkingErrors();

		// fucntion for receiving uniform locations, calls glGetUniformLocation if it isn't cached already
		int getUniformLocation(const std::string& name);
	};
}
