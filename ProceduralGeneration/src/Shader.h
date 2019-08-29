#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>

#include <vector>

class Shader
{
public:
	// the shader program ID;
	unsigned int ID;

	// gets the source code from strings and builds it
	Shader(const std::vector<unsigned int>& shaderIDs) {
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
	void use() {
		glUseProgram(ID);
	}

	// utility uniform functions
	void setBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

private:
	// utility function for checking linking errors.
	// ------------------------------------------------------------------------
	void checkLinkingErrors()
	{
		int success;
		char infoLog[1024];
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: PROGRAM" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
};