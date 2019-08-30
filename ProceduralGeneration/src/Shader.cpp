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
void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}


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