#include "ShaderManager.h"

#include "glad/glad.h"

#include <unordered_map>
#include <string>
#include <iostream>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "Debug.h"
using namespace Engine;

std::unordered_map<std::string, unsigned int> ShaderManager::shaderIndex;

void ShaderManager::setupShaderSource() {
	std::string path = "res/shaders";

	if (std::filesystem::is_directory(path) == false) {
		path = "src/shaders";
	}

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_directory()) continue;

		std::string shaderCode;
		std::ifstream shaderFile;
		
		// ensure ifstream objects can throw exceptions:
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		
		try {
			shaderFile.open(entry.path());
			std::stringstream shaderStream;

			shaderStream << shaderFile.rdbuf();

			shaderFile.close();

			shaderCode = shaderStream.str();
		}
		catch (std::ifstream::failure e) {
			LogManager::LogError("Shader file not successfully read");
		}

		// get the name of the file
		std::string entryPath = entry.path().string();
		int index = (int)entryPath.size() - 1;
		for (; index >= 0; index--) {
			if (entryPath[index] == '\\' || entryPath[index] == '/') {
				index++;
				break;
			}
		}
		std::string name = entryPath.substr(index);

		compileShader(shaderCode, name);
	}
}

unsigned int ShaderManager::getShaderID(const std::string& shaderName) {
	return shaderIndex[shaderName];
}

void ShaderManager::compileShader(const std::string& shaderCode, const std::string& name) {
	LogManager::Log("Compiling shader: " + name);

	// the last 4 letters
	std::string extension = name.substr(name.size() - 4);
	
	// glsl, frag, vert, geom, comp, tese, tesc
	if		(extension == "vert") shaderIndex[name] = glCreateShader(GL_VERTEX_SHADER);
	else if (extension == "frag") shaderIndex[name] = glCreateShader(GL_FRAGMENT_SHADER);
	else if (extension == "geom") shaderIndex[name] = glCreateShader(GL_GEOMETRY_SHADER);
	else if (extension == "comp") shaderIndex[name] = glCreateShader(GL_COMPUTE_SHADER);
	else if (extension == "tese") shaderIndex[name] = glCreateShader(GL_TESS_EVALUATION_SHADER);
	else if (extension == "tesc") shaderIndex[name] = glCreateShader(GL_TESS_CONTROL_SHADER);

	const char* shaderCodeC = shaderCode.c_str();
	glShaderSource(shaderIndex[name], 1, &shaderCodeC, NULL);
	glCompileShader(shaderIndex[name]);
	
	checkCompileErrors(shaderIndex[name], name);
}

void ShaderManager::checkCompileErrors(unsigned int shader, const std::string& name) {
	int success;
	char infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		LogManager::LogError("Failed to compile file: " + name + "\n" + std::string(infoLog) + "\n -- --------------------------------------------------- -- ");
	}
}

void ShaderManager::cleanup() {
	for (const auto& index : shaderIndex) {
		LogManager::Log("Deleting shaders: " + index.first);
		glDeleteShader(index.second);
	}
}