#include "Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glad/glad.h"

#include <string>
#include <iostream>
#include <vector>

#include "Debug.h"
using namespace Engine;

Shader::Shader(const std::vector<unsigned int>& shaderIDs) {
	this->ID = glCreateProgram();

	for (const auto& id : shaderIDs) {
		glAttachShader(this->ID, id);
	}

	glLinkProgram(this->ID);
	checkLinkingErrors();
}

Shader::~Shader() {
	glDeleteProgram(this->ID);
}

void Shader::use() {
	glUseProgram(this->ID);
}

void Shader::unbind() {
	glUseProgram(0);
}

// utility uniform functions
// ---------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) {
	glUniform1i(getUniformLocation(name), (int)value);
}
void Shader::setInt(const std::string& name, int value) {
	glUniform1i(getUniformLocation(name), value);
}
void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(getUniformLocation(name), value);
}
// ---------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) {
	glUniform2fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) {
	glUniform2f(getUniformLocation(name), x, y);
}
// ---------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) {
	glUniform3fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) {
	glUniform3f(getUniformLocation(name), x, y, z);
}
// ---------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) {
	glUniform4fv(getUniformLocation(name), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
	glUniform4f(getUniformLocation(name), x, y, z, w);
}
// ---------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& value) {
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::setMat3(const std::string& name, const glm::mat3& value) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::setMat4(const std::string& name, const glm::mat4& value) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------


void Shader::checkLinkingErrors() {
	int success;
	char infoLog[1024];
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->ID, 1024, NULL, infoLog);
		std::cout << "Error linking program" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
}

int Shader::getUniformLocation(const std::string& name) {
	if (this->uniformLocations.find(name) == this->uniformLocations.end()) {
		int location = glGetUniformLocation(this->ID, name.c_str());
		this->uniformLocations[name] = location;
		return location;
	}
	return this->uniformLocations[name];
}
