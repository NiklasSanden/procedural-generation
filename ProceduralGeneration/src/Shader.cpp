#include "Shader.h"

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <vector>

// gets the source code from strings and builds it
Shader::Shader(const std::vector<unsigned int>& shaderIDs) {
	// shader Program
	ID = glCreateProgram();

	// attach all shaders
	for (const auto& id : shaderIDs) {
		glAttachShader(ID, id);
	}

	// link shader program and print linking errors if any
	glLinkProgram(ID);
	checkLinkingErrors();
}

// use/activate the shader
void Shader::use() {
	glUseProgram(ID);
}

// utility uniform functions
// ---------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
// ---------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ---------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ---------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
// ---------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& value) const {
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void Shader::setMat3(const std::string& name, const glm::mat3& value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------


// utility function for checking linking errors.
// ------------------------------------------------------------------------
void Shader::checkLinkingErrors() {
	int success;
	char infoLog[1024];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: PROGRAM" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
}