#include "ResourceManager.h"

#include "stb_image.h"

#include <iostream>
#include <string>

using namespace Engine;

// Instantiate static variables
std::map<std::string, Texture2D> ResourceManager::textures;

Texture2D ResourceManager::loadTexture(const char* file, bool alpha, std::string name) {
	std::string filePath = "res/textures/" + std::string(file);
	textures[name] = loadTextureFromFile(filePath.c_str(), alpha);
	return textures[name];
}

Texture2D ResourceManager::getTexture(std::string name) {
	return textures[name];
}

void ResourceManager::cleanup() {
	// (Properly) delete all textures
	for (auto texture : textures)
		glDeleteTextures(1, &texture.second.ID);
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha) {
	// Make sure to flip them
	stbi_set_flip_vertically_on_load(true);
	// Create Texture object
	Texture2D texture;
	if (alpha)
	{
		texture.internalFormat = GL_RGBA;
		texture.imageFormat = GL_RGBA;
	}

	// Load image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	// Now generate texture
	if (data) {
		texture.generate(width, height, data);
		// And finally free image data
		stbi_image_free(data);
	}
	else { // Failed to read the image
		std::cout << "stb failed to read image: " << file << std::endl;
	}
	return texture;
}
