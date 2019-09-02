#pragma once

#include "Texture2D.h"

#include <glad/glad.h>

#include <map>
#include <string>

// A static singleton ResourceManager class that hosts several
// functions to load Textures. Each loaded texture is also 
// stored for future reference by string handles. All functions 
// and resources are static and no public constructor is defined.
namespace Engine {
	class ResourceManager {
	public:
		// Resource storage
		static std::map<std::string, Texture2D> textures;
		// Loads (and generates) a texture from file
		static Texture2D loadTexture(const char* file, bool alpha, std::string name);
		// Retrieves a stored texture
		static Texture2D getTexture(std::string name);
		// Properly de-allocates all loaded resources
		static void cleanup();
	private:
		// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
		ResourceManager() { }
		// Loads a single texture from file
		static Texture2D loadTextureFromFile(const char* file, bool alpha);
	};
}
