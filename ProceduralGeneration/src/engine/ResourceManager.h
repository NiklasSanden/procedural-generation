#pragma once

namespace Engine {
	class Texture2D;
	class Shader;
}

#include <unordered_map>
#include <string>
#include <vector>

// A static singleton ResourceManager class that hosts several
// functions to load Textures and shaderprograms. Each loaded resource is also 
// stored for future reference by string handles. All functions 
// and resources are static and no public constructor is defined.
namespace Engine {
	class ResourceManager {
	public:
		static std::unordered_map<std::string, Texture2D*> textures;
		static std::unordered_map<std::string, Shader*> shaderPrograms;
		
		// Loads (and generates) all texture files in the "res/textures/" folder
		static void loadAllTextures();
		static Texture2D* loadTexture(const char* file, bool alpha, std::string& name);
		static Texture2D* getTexture(std::string& name);
		
		// Creates a shaderProgram if the name doesn't already exist
		static Shader* createShaderProgram(std::vector<std::string>& files, std::string& name);
		static Shader* getShaderProgram(std::string& name);
		
		static void cleanup();
	private:
		// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
		ResourceManager() {};

		static Texture2D* loadTextureFromFile(const char* file, bool alpha);
	};
}
