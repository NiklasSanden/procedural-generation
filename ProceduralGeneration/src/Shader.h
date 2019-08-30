#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>

class Shader
{
public:
	// the shader program ID;
	unsigned int ID;

	// gets the source code from strings and builds it
	Shader(const std::vector<unsigned int>& shaderIDs);

	// use/activate the shader
	void use();

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

private:
	// utility function for checking linking errors.
	// ------------------------------------------------------------------------
	void checkLinkingErrors();
};