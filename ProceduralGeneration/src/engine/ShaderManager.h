#pragma once
#include <string>
#include <map>
#include <vector>

// A static singleton ShaderManager class that hosts several
// functions to load Shaders. Each loaded shader is also 
// stored for future reference by string handles. All functions 
// and resources are static and no public constructor is defined.
namespace Engine {
	class ShaderManager {
	private:
		// Private constructor, that is we do not want any actual resource manager objects.Its membersand functions should be publicly available(static).
		ShaderManager() {};

		// fileNames and sourceCode are used when not reading the source code from files
		static std::vector<std::string> fileNames;
		static std::vector<std::string> sourceCode;
		static std::map<std::string, unsigned int> shaderIndex;

		static void compileShader(std::string& shaderCode, std::string& name);

		static void checkCompileErrors(unsigned int shader, std::string& name);

	public:
		static void setupShaderSource();

		static unsigned int getShaderID(std::string shaderName);

		static void cleanup();
	};
}