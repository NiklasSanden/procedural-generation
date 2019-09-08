#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// A static singleton ShaderManager class that hosts several
// functions to load Shaders. Each loaded shader is also 
// stored for future reference by string handles. All functions 
// and resources are static and no public constructor is defined.
namespace Engine {
	class ShaderManager {
	// Although this is supposed to be a static singleton, this is the exception...
	private:
		// FileNames and sourceCode are used when not reading the source code from files
		// They will not be static so that they don't have to sit in memory throughout the entire application's lifespan.
		// They will only be used once and then deallocated when the object gets removed from the stack
		std::vector<std::string> fileNames;
		std::vector<std::string> sourceCode;
		ShaderManager(); // The constructor can still be private since it will only be called from within the setupShaderSource function

	// static
	private:
		static std::unordered_map<std::string, unsigned int> shaderIndex;

		static void compileShader(const std::string& shaderCode, const std::string& name);

		static void checkCompileErrors(unsigned int shader, const std::string& name);

	public:
		static void setupShaderSource();

		static unsigned int getShaderID(const std::string& shaderName);

		static void cleanup();
	};
}