#pragma once
#include <string>
#include <unordered_map>

class ShaderFileHandler {
private:
	// fileNames and sourceCode are used when not reading the source code from files
	static std::vector<std::string> fileNames;
	static std::vector<std::string> sourceCode;
	static std::unordered_map<std::string, unsigned int> shaderIndex;

	static void compileShader(std::string& shaderCode, std::string& name);

	static void checkCompileErrors(unsigned int shader, std::string& name);

public:
	static void setupShaderSource();

	static unsigned int getShaderID(std::string shaderName);

	static void cleanup();
};