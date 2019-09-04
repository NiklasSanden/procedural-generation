#pragma once

#include "Texture2D.h"
#include "Shader.h"

#include <glad/glad.h>

#include <map>
#include <string>
#include <vector>

// A static singleton ResourceManager class that hosts several
// functions to load Textures and shaderprograms. Each loaded resource is also 
// stored for future reference by string handles. All functions 
// and resources are static and no public constructor is defined.
namespace Engine {
	class ResourceManager {
	public:
		// Resource storage
		static std::map<std::string, Texture2D*> textures;
		static std::map<std::string, Shader*> shaderPrograms;
		// Loads (and generates) a texture from file
		static Texture2D* loadTexture(const char* file, bool alpha, std::string name);
		// Retrieves a stored texture
		static Texture2D* getTexture(std::string name);
		// Creates a shaderProgram if the name doesn't already exist
		static Shader* createShaderProgram(std::vector<std::string> files, std::string name);
		// Gets a shaderProgram based on name
		static Shader* getShaderProgram(std::string name);
		// Properly de-allocates all loaded resources
		static void cleanup();
	private:
		// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
		ResourceManager() {};
		// Loads a single texture from file
		static Texture2D* loadTextureFromFile(const char* file, bool alpha);
	};
}
